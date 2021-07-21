#define _CRT_NONSTDC_NO_WARNINGS 1
#define _CRT_SECURE_NO_WARNINGS 1
#include "archive.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

// Forward Declarations

Archive_t Archive_Create(char*);
Archive_t Archive_Full_Write(Archive_t* master_ar, char* ar_name);
void Archive_Append_File(Archive_t*, char*);

void ArFile_Write_Header(Archive_File_t*);

void Archive_Delete_File(Archive_t*, char*);

void Archive_Append_Directory(Archive_t*);

void Archive_Write_Master(Archive_t*, Archive_File_t*);

void Archive_Help();
/**
 * @brief 
 * 
 * @param master_ar 
 * @return Archive_t 
 */
Archive_t Archive_Full_Write(Archive_t* master_ar, char* ar_name){
    Archive_t new_ar = Archive_Create(ar_name);
    Archive_File_t* head = master_ar->head; // temp head.
    


    if(head == NULL){ // check if shit exists.
        fprintf(stderr, "The list is empty my guy.\n");
        exit(-1);
    }

    while(head->next != NULL){ // traverse til we get to the end
        Archive_Append_File(&new_ar, head->archive_file_name); // Keep adding the files to the new archive.
        head = head->next;
    }
    return new_ar;
}

/**
 * @brief Given the name of the archive, and the file you want to make the master archive in. 
 * * DOES NOT SET UP THE FIRST ARCHIVE FILE. DO THAT IN A DIFFERENT FUNCTION.
 * 
 * @param arch_name 
 * @param master_file_name 
 * @return Archive_t 
 */
Archive_t Archive_Create(char* master_file_name){
    // Open the master archive, assign a name, allocate the archive file list.
    Archive_t NewArchive; // Make a new archive.
    memcpy(NewArchive.archive_name, master_file_name, NAME_SIZE); // Copy over the name to the struct.

    NewArchive.total_files = 0;
    NewArchive.master_archive = fopen(master_file_name, "r+"); // Create the master archive with read write invasive mode. (Deletes archive file if it already exists)
    NewArchive.head = malloc(sizeof(Archive_File_t)); //Allocate the list inside of the master archive.
    if(fclose(NewArchive.master_archive) != 0){ // stream didn't close correctly.
        fprintf(stderr, "Stream not successfully closed");
    }

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

    ArFile_Write_Header(new_ar_file); // Write the file headers. 

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
    FileInfo* fileInfo;
    
    fileInfo = malloc(sizeof(FileInfo));
    // I hope I don't need an open stream for this.
    int error_flag = stat(p_ar_file->archive_file_name, fileInfo);

    // * int fileno(FILE *stream); Useful, gets a file descriptor from a FILE* stream, returns -1 if failed.
    if (error_flag == -1){ 
        perror("Error occured in ArFile_Write_Header with FILE* to file descriptor conversion\n");
        exit(-1);
    }
    
    // ? Sets all of the header buffers to spaces.
    memset(&p_ar_file->arch_head, ' ', sizeof(Ar_hdr_t)); // ? sets all of the ar fields to spaces.
    // ! However, this memset only works because we've made a struct containing the archive header stuff.
    // ! If you don't have this, then youd have to memset each of these to be empty with spaces, using the size of the field.

    // TODO: date, uid, gid, mode, size, fmag
    char mem_buff[32]; // Conversion space.
    // * For name.
    sprintf(mem_buff, "%s/", p_ar_file->archive_file_name);
    memcpy(p_ar_file->arch_head.ar_name, mem_buff, strlen(mem_buff)); // Get the file name into the header.

    printf("Written Name: %s\n", p_ar_file->arch_head.ar_name);

    // file.txt/                 s
    // [file.txt/                ]

    // * For date.
    sprintf(mem_buff, "%ld", fileInfo->st_mtime); // format for long 
    memcpy(p_ar_file->arch_head.ar_date, mem_buff, strlen(mem_buff));
    
    // * For mode.
    sprintf(mem_buff, "%o", fileInfo->st_mode);  // format the mode into octal for 644 etc.
    memcpy(p_ar_file->arch_head.ar_mode, mem_buff, strlen(mem_buff));
    
    // * For UID
    sprintf(mem_buff, "%d", (int)fileInfo->st_uid);  // format a uid as a long into the buffer
    memcpy(p_ar_file->arch_head.ar_uid, mem_buff, strlen(mem_buff));

    // * For GID
    sprintf(mem_buff, "%d", (int)fileInfo->st_gid);
    memcpy(p_ar_file->arch_head.ar_gid, mem_buff, strlen(mem_buff));

    
    // * For ar size.
    sprintf(mem_buff, "%ld", fileInfo->st_size);  // format the mode into octal for 644 etc.
    memcpy(p_ar_file->arch_head.ar_size, mem_buff, strlen(mem_buff));
    
    // * Pretty sure I can directly copy over the ARFMAG string into the buffer.
    memcpy(p_ar_file->arch_head.ar_fmag, ARFMAG, AR_FMAG_SIZE);
    free(fileInfo); // free the stat buffer that we allocated.
}

/**
 * @brief Given a pointer to an archive file, free all of the resources attached to it. Returns 1 if succeeded, 0 if failed.
 * 
 * @param p_ar_file 
 * @return int 
 */
int Archive_Free_File(Archive_File_t* p_ar_file){
    // TODO: Free file info, free current 
    
    free(p_ar_file->archive_file_name); // free the name we had when we appended.
    free(p_ar_file); // Free the actual file.
    
    return 1;
}

//TODO: Complete deletion from archive.
/**
 * @brief Deletes a file from the archive, doesn't rewrite the archive.
 * 
 * @param master_ar 
 * @param target_file 
 */
void Archive_Delete_File(Archive_t* master_ar, char* target_file){
    // Go through the linked list and find the name of the file to delete. 
    
    Archive_File_t* temp_head = master_ar->head; // temp head.
    Archive_File_t* prev;

    // ? Search for the name to be deleted.
    printf("Name given: %s, name to delete: %s\n", temp_head->arch_head.ar_name, target_file);
    // if the head is the key delete it & drop next ptr.
    if(temp_head != NULL && temp_head->arch_head.ar_name == target_file){
        master_ar->head = temp_head->next; // change head
        Archive_Free_File(temp_head); // Free the old head.
        return;
    }
    // Search for the name to delete.
    while(temp_head != NULL && temp_head->arch_head.ar_name != target_file){
        prev = temp_head; // Keep track of previous
        temp_head = temp_head->next; // traverse to next one.
    }
    // If the file never existed. Give an error.
    if(temp_head == NULL){
        fprintf(stderr, "File was not found in the archive!\n");
        return;
    }
    // Unlink our node from the list.
    prev->next = temp_head->next;
    Archive_Free_File(temp_head);
    
    printf("Deleted: %s\n", target_file);
}

//TODO: Just use Ar Append file for the whole file list in the dir.
void Archive_Append_Directory(Archive_t* master_ar){
    printf("Directory Append WIP\n");
}

/**
 * @brief Get the hdr size object, magic C stuff :shrug:
 * 
 * @param h 
 * @return long 
 */
long get_hdr_size(Ar_hdr_t* h){
    char buf[32];
    memcpy(buf, h->ar_size, AR_FSIZE_SIZE);
    buf[AR_FSIZE_SIZE] = '\0';
    return strtol(buf, NULL, AR_FSIZE_SIZE);
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
    // ? Allocate this with enough space to hold the size of the ar file and vibe.
    long file_size = get_hdr_size(&new_ar_file->arch_head); // Get the file size as an int. 
    char* data_buffer = malloc(sizeof(char) * file_size); // make enough space to copy over the contents of the file.

    new_ar_file->archive_file = fopen(new_ar_file->archive_file_name, "r");
    master_ar->master_archive = fopen(new_ar_file->archive_file_name, "a+"); // open file stream to append.

    // Open a stream to the master archive, if the stream is open, then JUST write to the stream, don't attempt to open a new one.
    if(master_ar->master_archive != NULL){ // The stream is open.
        printf("Stream is open in Archive_Write_Master.\n");
    }else{
        perror("It seems that the master archive stream is closed in Archive_Write_Master, or something went wrong and the fopen in the Archive_Create isn't persistent. \n");
    }
    
    // ? Write all of our current file data including the header into the data buffer.
    
    fwrite(&new_ar_file->arch_head, AR_HEADER_SIZE, 1, master_ar->master_archive);

    // ? Reading from the 
    fread(data_buffer, file_size, 1, new_ar_file->archive_file);
    
    fwrite(data_buffer, file_size, 1, master_ar->master_archive);
    if(file_size % 2){ // if its odd/even
        fputc('\n', master_ar->master_archive);
    }
    fclose(new_ar_file->archive_file);
    fclose(master_ar->master_archive);
    free(data_buffer); // Freed the data buffer.
}

/**
 * @brief This is an annoying ass function, but it essentially sees if any of the flags have been set at all, if they havent been set, then the archive has no flags which isn't correct.
 * 
 * @param master_ar 
 * @return true 
 * @return false 
 */
bool Archive_No_Flags(Archive_t* master_ar){
    if(master_ar->ar_flags.option_A + master_ar->ar_flags.option_d + master_ar->ar_flags.option_q + master_ar->ar_flags.option_t + master_ar->ar_flags.option_t + master_ar->ar_flags.option_v + master_ar->ar_flags.option_w + master_ar->ar_flags.option_x == 0){ // If none of the flags have been set at all, I couldnt find a clean way to do this.
        return true;
    }else{
        return false;
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