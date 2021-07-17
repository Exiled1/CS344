#include "archive_header.h"
#include "archive.h"
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
    NewArchive.archive_name = arch_name;
    strncpy(NewArchive.archive_name, arch_name, NAME_SIZE); // Copy over the name to the struct.

    NewArchive.total_files = 0;
    NewArchive.master_archive = fopen(master_file_name, "rw"); // Create the master archive.
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

    new_ar_file->archive_file_name = malloc(sizeof(char) * (strlen(file_name) + 1));

    strcpy(new_ar_file->archive_file_name, file_name);
    
    new_ar_file->archive_file = fopen(new_ar_file->archive_file_name, "r"); // read the file, then write the contents to the master archive.

    if(new_ar_file->archive_file == NULL){ // If the file doesn't open, then cause a read error.
        perror("Read error occured, please check if the file exists. Exiting with error code.\n");
        exit(1); 
    }

    ArFile_Write_Header(new_ar_file); // Write the file headers. Position matters.

    Ar_Write_Master(p_archive, new_ar_file); // Write the file + ar header contents to the master archive.

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

void ArFile_Write_Header(Archive_File_t* p_ar_file){
    
}

void Ar_Append_Directory(Archive_t* master_ar){
    printf("Directory Append WIP\n");

}

void Ar_Write_Master(Archive_t* master_ar, Archive_File_t* new_ar_file){
    printf("Master Write WIP\n");
}

