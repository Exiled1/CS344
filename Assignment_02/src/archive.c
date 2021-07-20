#define _POSIX_C_SOURCE 2
#include "archive.h"


// Forward Declarations

Archive_t Archive_Create(char*, char*);

void Archive_Append_File(Archive_t*, char*);

void ArFile_Write_Header(Archive_File_t*);

void Archive_Delete_File(Archive_t*, char*);

void Archive_Append_Directory(Archive_t*);

void Archive_Write_Master(Archive_t*, Archive_File_t*);

void Archive_Help();

/**
 * @brief Given the name of the archive, and the file you want to make the master archive in. 
 * * DOES NOT SET UP THE FIRST ARCHIVE FILE. DO THAT IN A DIFFERENT FUNCTION.
 * 
 * @param arch_name 
 * @param master_file_name 
 * @return Archive_t 
 */
Archive_t Archive_Create(char* arch_name, char* master_file_name){
    // Open the master archive, assign a name, allocate the archive file list.
    Archive_t NewArchive; // Make a new archive.
    strncpy(NewArchive.archive_name, arch_name, NAME_SIZE); // Copy over the name to the struct.

    NewArchive.total_files = 0;
    NewArchive.master_archive = fopen(master_file_name, "w+"); // Create the master archive with read write invasive mode. (Deletes archive file if it already exists)
    NewArchive.head = malloc(sizeof(Archive_File_t)); //Allocate the list inside of the master archive.
    return NewArchive;
}

/**
 * @brief Given a file name, this appends a new file into the master archive. Assumes that each file is uniquely named. Also, this has the side effect of assigning header values when appending.
 * 
 * @param p_archive 
 * @param file_name 
 */
void Archive_Append_File(Archive_t* p_archive, char* file_name){
    Archive_File_t* new_ar_file = malloc(sizeof(Archive_File_t)); // Creating a new archive file node.

    new_ar_file->archive_file_name = malloc(sizeof(char) * (strlen(file_name) + 1)); // Create enough space in the struct to fit in the file name PLUS the null byte.

    strcpy(new_ar_file->archive_file_name, file_name); // Copy over the file name into the object.
    
    new_ar_file->archive_file = fopen(new_ar_file->archive_file_name, "r"); // read the file, then write the contents to the master archive.

    if(new_ar_file->archive_file == NULL){ // If the file doesn't open, then cause a read error.
        perror("Read error occured, please check if the file exists. Exiting with error code.\n");
        exit(1); 
    }

    ArFile_Write_Header(new_ar_file); // Write the file headers. Position matters.

    Archive_Write_Master(p_archive, new_ar_file); // Write the file + ar header contents to the master archive.

    // * EVERYTHING BELOW HERE IS TO ADD THE AR FILE NODE INTO THE LINKED LIST.
    // You have a new node made and the data is added in.
    // TODO: Linked list append algo.

    if(p_archive->head == NULL){ // Check if the linked list exists, if not initialize it.
        p_archive->head = new_ar_file; // Make the new ar file the head of the archive list.
        return;
    } 

    // If the head is filled, traverse to the end of the list.
    Archive_File_t* temp_head = p_archive->head; // Make a temporary head for traversal.
    while(temp_head->next != NULL){
        temp_head = temp_head->next;
    }

    // We're at the end of the list. Set the next node to null .
    temp_head->next = new_ar_file;
    return;
}

/**
 * @brief Write all of the relevant header data to the archive file.
 * 
 * @param p_ar_file 
 */
void ArFile_Write_Header(Archive_File_t* p_ar_file){
    FileInfo fileInfo;
    
    // * int fileno(FILE *stream); Useful, gets a file descriptor from a FILE* stream, returns -1 if failed.
    //int ar_file_desc = fileno(p_ar_file->archive_file); // FD number from the struct stream.
    // If either the fileno or the stat functions fail, order matters for the if, since stat runs first, dont swap the order.
    if (stat(p_ar_file->archive_file_name, &fileInfo) == -1){ 
        perror("Error occured in ArFile_Write_Header with FILE* to file descriptor conversion\n");
        exit(-1);
    }
    // After all our initilization is done, assign the relevant header data to the archive file.
    // TODO: date, uid, gid, mode, size, fmag, DONE!!
    memcpy(p_ar_file->arch_head.ar_name, p_ar_file->archive_file_name, AR_NAME_SIZE); // Get the file name into the header.
    memcpy(p_ar_file->arch_head.ar_date, &fileInfo.st_mtime, AR_DATE_SIZE);
    memcpy(p_ar_file->arch_head.ar_uid, &fileInfo.st_uid, AR_UID_SIZE);
    memcpy(p_ar_file->arch_head.ar_gid, &fileInfo.st_gid, AR_GID_SIZE);
    memcpy(p_ar_file->arch_head.ar_mode, &fileInfo.st_mode, AR_MODE_SIZE);
    memcpy(p_ar_file->arch_head.ar_size, &fileInfo.st_size, AR_FSIZE_SIZE);
    memcpy(p_ar_file->arch_head.ar_fmag, ARFMAG, AR_FMAG_SIZE);

}

//TODO: Complete deletion from archive.
void Archive_Delete_File(Archive_t* master_ar, char* target_file){
    printf("Archive File Delete WIP.\n");
}

//TODO: Just use Ar Append file for the whole file list in the dir.
void Archive_Append_Directory(Archive_t* master_ar){
    printf("Directory Append WIP\n");
}


/**
 * @brief This method takes a master archive and an archive file that you
 *  want to add to the master archive file and writes it to the master archive file.
 *  This method does not go through the master archive linked list though, since it's
 *  called whenever we append a new file. Also adds in the order of [HEADER][FILE] into the master archive.
 * 
 * @param master_ar 
 * @param new_ar_file 
 */
void Archive_Write_Master(Archive_t* master_ar, Archive_File_t* new_ar_file){
    char data_buffer[WRITE_BUFFER_SIZE]; // Keep a data buffer to read and write file data into the master archive file.

    // Open a stream to the master archive, if the stream is open, then JUST write to the stream, don't attempt to open a new one.
    if(ftell(master_ar->master_archive) >= 0){ // The stream is open.
        printf("Stream is open in Archive_Write_Master.\n");
    }else{
        perror("It seems that the master archive stream is closed in Archive_Write_Master, or something went wrong and the fopen in the Archive_Create isn't persistent. \n");
    }
    // Assign the stored header data before 
    fputs(ARMAG, master_ar->master_archive);
    fputs(new_ar_file->arch_head.ar_date, master_ar->master_archive);
    fputs(new_ar_file->arch_head.ar_uid, master_ar->master_archive);
    fputs(new_ar_file->arch_head.ar_mode, master_ar->master_archive);
    fputs(new_ar_file->arch_head.ar_size, master_ar->master_archive);
    fputs(new_ar_file->arch_head.ar_fmag, master_ar->master_archive);

    // While we haven't reached the end of the archive file stream write to the master archive file. This code snippet will write the contents of the file, but not the header.
    while(fgets(data_buffer, WRITE_BUFFER_SIZE, new_ar_file->archive_file) != NULL){
        fputs(data_buffer, master_ar->master_archive);
    }

}


/**
 * @brief This is just something that prints helpful instructions on using the myar tool.
 * 
 */
void Archive_Help(){
    printf("Usage: myar [-][OPTIONS] archive_name [FILE(S) ...]\n");
    printf("Desc: 'myar' saves any number of files into an archive file, it's also able to extract files from a 'myar' archive file.\n\n");
    printf("Examples:\n");
    printf("\tmyar -q archive.ar foo.txt bar.txt  # Create an archive.ar from the files foo and bar.\n");
    printf("Option flags:\n");
    printf("\t-q, Quickly append files to an archive.\n");
    printf("\t-x, Extract named files from an archive.\n");
    printf("\t-t, Print a CONCISE table of contents of the archive.\n");
    printf("\t-v, Print a VERBOSE table of contents of the archive.\n");
    printf("\t-d, Delete named files from an archive.\n");
    printf("\t-A, Quickly append all 'regular' files in the current directory except the archive itself and binary files.\n");

}

