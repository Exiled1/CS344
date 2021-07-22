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
    FILE* ar_write = fopen(ar_name, "w+");
    Archive_File_t* head = master_ar->head;

    if(head->archive_file_name == NULL){ // check if shit exists.
        fprintf(stderr, "The list is empty my guy.\n");
        exit(-1);
    }

    while(head){ // traverse til we get to the end
        // Archive_Append_File(&new_ar, head->archive_file_name); // Keep adding the files to the new archive.
        Archive_Write_Master(master_ar, head); // Write the file + ar header contents to the master archive.
        head = head->next;
    }
    return *master_ar;
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
    
    memcpy(NewArchive.archive_name, master_file_name, strlen(master_file_name)); // Copy over the name to the struct.

    NewArchive.total_files = 0;
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
    new_ar_file->next = NULL;
    printf("File name thing: %s\n", file_name);
    // ! Not null terminated
    new_ar_file->archive_file_name = malloc(sizeof(char) * (strlen(file_name) + 1)); // Create enough space in the struct to fit in the file name PLUS the null byte.
    
    strcpy(new_ar_file->archive_file_name, file_name); // Copy over the file name into the object.

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
    
    // free(p_ar_file->archive_file_name); // free the name we had when we appended.
    free(p_ar_file); // Free the actual file.
    
    return 1;
}
/**
 * @brief Get the file numbers from a archive file.
 * 
 * @param archive 
 * @param ar_name 
 * @return long 
 */
long ar_get_file_num(Archive_t* archive, char* ar_name){
    Ar_hdr_t header;
    int bytes_read = SARMAG;
    long file_num = 0;
    archive->master_archive = fopen(ar_name, "r");
    char header_buff[AR_HEADER_SIZE];
    fseek(archive->master_archive, 0, SEEK_END);
    int file_size = ftell(archive->master_archive); 
    while(bytes_read < file_size){
        fseek(archive->master_archive, bytes_read, SEEK_SET); // 8 bytes from the beginning, skipping armag.
        memset(header_buff, ' ', AR_HEADER_SIZE); // empty buffer.
        fread(header_buff, AR_HEADER_SIZE, 1, archive->master_archive); // read 60 bytes of data from stream.
        memcpy(&header, header_buff, AR_HEADER_SIZE); // Sets the header to the data.
        bytes_read = bytes_read + AR_HEADER_SIZE + get_hdr_size(&header); // Hopefully skips over the file content.
        file_num++;
    }
    return file_num;
}

/**
 * @brief Assumes you already added the data into the archive file.
 * 
 * @param archive 
 * @param ar_file 
 */
void good_append(Archive_t* archive, Archive_File_t* ar_file){
    Archive_File_t* new_node = malloc(sizeof(Archive_File_t));
    Archive_File_t* temp_node = archive->head;
    //Archive_File_t* last = archive->head;
    long ar_fsize = get_hdr_size(&ar_file->arch_head);
    memcpy(new_node, ar_file, sizeof(Archive_File_t));

    new_node->next = NULL;

    if(archive->head->archive_file_name == 0x0){
        printf("Something happend\n");
        archive->head = new_node;
        return;
    }

    while(temp_node->next != NULL){
        temp_node = temp_node->next;
    }

    temp_node->next = new_node;
    return;

}

/**
 * @brief This is not for extraction, this is for reconstruction of the archive linked list. The archive must be Created beforehand with Archive_Create.
 * 
 * @param archive 
 * @param archive_name 
 */
void Archive_Retrieve(Archive_t* archive, char* archive_name){
    // * The point of this is given the old archive name, construct a new archive linked list out of the contents of the old one.
    char armag[SARMAG]; // yoink the magic numbers.
    char header_buff[AR_HEADER_SIZE]; // make a thing to fit the header.
    char ar_fname[AR_NAME_SIZE]; // make buffer for the name.
    char ssize[AR_HEADER_SIZE]; // more size data.
    long file_size;
    int curr_byte = 0;
    int curr_hbyte = 0;
    int byte;
    Ar_hdr_t header;
    Archive_File_t append_file;
    // Archive_t ar_copy = Archive_Create(archive_name);
    Archive_File_t new_file; // head ref
    Archive_File_t* temp = archive->head; // TODO: WORK WITH THE LAST NODE AND CONNECT THESE CORRECTLY.
    archive->master_archive = fopen(archive_name, "r");
    if (archive->master_archive == NULL){
        perror("Error occured with reading data.\n");
        exit(-1);
    }
    // Check if the file is an archive file.
    fseek(archive->master_archive, -1, SEEK_SET); // Go to beginning of file.
    memset(armag, ' ', SARMAG);
    fread(armag, SARMAG, 1, archive->master_archive); // read the first 8 bytes.
    // printf("Strings: %s, %s", armag, ARMAG);

    if(memcmp(armag, ARMAG, SARMAG) == 0){ // The bytes are the same.
        printf("This is a valid archive file. Extracting data...\n");
    }else{
        printf("Strings: %s, %s", armag, ARMAG);
        printf("Not an ar file or doesn't exist.\n");
        exit(-1);
    }

    char* file_data; // malloc this to the file size.
    // Append this.

    // If the head is filled, traverse to the end of the list.
    for(int i = 0; i < archive->total_files; i++){
        memset(header_buff, ' ', AR_HEADER_SIZE); // empty buffer.
        fread(header_buff, AR_HEADER_SIZE, 1, archive->master_archive); // read 60 bytes of data from stream.
        memcpy(&header, header_buff, AR_HEADER_SIZE); // copy the contents of the buffer into the current head memory block. 
        
        // * Copies header into struct.
        long file_size = get_hdr_size(&header);

        // * Copy file data.
        // ! If this breaks, change to a plus 1.
        file_data = malloc(sizeof(char) * file_size); // allocates a buffer of the file contents.
        fread(file_data, file_size, 1, archive->master_archive); // read the file contents into the buffer.
        
        new_file.file_contents = malloc(sizeof(char) * file_size);

        memcpy(new_file.file_contents, file_data, sizeof(char) * file_size); // hopefully copies correctly, idfk.
        memcpy(&new_file.arch_head, &header, AR_HEADER_SIZE);
        new_file.archive_file_name = malloc(sizeof(char) * AR_NAME_SIZE);
        char* thing = strstr(new_file.arch_head.ar_name, "/");
        *thing = 0;
        memcpy(new_file.archive_file_name, new_file.arch_head.ar_name, strlen(new_file.arch_head.ar_name));

        // TODO: assign header, then assign content.
        new_file.next = NULL;
        good_append(archive, &new_file);

    }

    return;

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

    master_ar->head->archive_file = fopen(target_file, "r"); // ? Idk why you did this
    Archive_File_t* temp_head = master_ar->head; // temp head.
    
    Archive_File_t* prev;

    // ? Search for the name to be deleted.
    printf("Name given: %s, name to delete: %s\n", temp_head->archive_file_name, target_file);
    // if the head is the key delete it & drop next ptr.
    if(temp_head != NULL && strcmp(temp_head->arch_head.ar_name, target_file) == 0){
        master_ar->head = temp_head->next; // change head
        Archive_Free_File(temp_head); // Free the old head.
        return;
    }
    // Search for the name to delete.
    while(temp_head != NULL && strcmp(temp_head->arch_head.ar_name, target_file)){
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
    master_ar->master_archive = fopen(master_ar->archive_name, "a+"); // open file stream to append.

    // Open a stream to the master archive, if the stream is open, then JUST write to the stream, don't attempt to open a new one.
    if(master_ar->master_archive != NULL || new_ar_file->archive_file != NULL){ // The stream is open.
        printf("Stream is open in Archive_Write_Master.\n");
    }else{
        perror("It seems that the master archive stream is closed in Archive_Write_Master, or something went wrong and the fopen in the Archive_Create isn't persistent. \n");
    }
    
    // ? Write all of our current file data including the header into the data buffer.
    
    fwrite(&new_ar_file->arch_head, AR_HEADER_SIZE, 1, master_ar->master_archive);

    // ? Reading from the  file into the buffer.
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