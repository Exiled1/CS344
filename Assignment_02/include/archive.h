#ifndef ASSIGNMENT_02_INCLUDE_ARCHIVE
#define ASSIGNMENT_02_INCLUDE_ARCHIVE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <utime.h>
#include <sys/stat.h>
#include "archive_header.h"



#define NAME_SIZE 32
#define WRITE_BUFFER_SIZE 256

typedef struct ar_flags Ar_flags;
typedef struct archive Archive_t;
typedef struct archive_file Archive_File_t;
typedef struct stat FileInfo; // Type definition of the stat struct from the stat library.

typedef struct ar_flags{
    bool option_q; // Keep track of option -q
    bool option_x; // Keep track of option -x
    bool option_t; // Keep track of option -t
    bool option_v; // Keep track of option -v
    bool option_d; // Keep track of option -d
    bool option_A; // Keep track of option -A
    bool option_w; // Keep track of option -w
}Ar_flags;

/**
 * @brief This describes the object of an archive. Contains the name of the archive. A list of the archive headers, and copies of the files in the file list.
 * ! If this ends up breaking, it might have to do with the malloc that we used to allcoate this.
 */
typedef struct archive_file{
    FILE* archive_file; // File pointer that we're grabbing.
    char* archive_file_name;
    Ar_hdr_t arch_head; // Header of the current archive file, has the permission mode.
    Archive_File_t* next; // Pointer to next archive file.
}Archive_File_t;

/**
 * @brief To allocate memory for the files, use p_name->list = malloc(sizeof(Archive_File_t)), this initializes your list, when appending, create a new archive file and add it to the end of the archive file list.
 * 
 */
typedef struct archive{
    Ar_flags ar_flags; // The flags associated with this archive. 
    char archive_name[NAME_SIZE]; // Name of the archive.
    int total_files; // Total files in the archive.
    FILE* master_archive; // The file where all of the archive files are written to.
    Archive_File_t* head; // keeps track of the archive files, malloc.
}Archive_t;

void Archive_Append_File(Archive_t* p_archive, char* file_name);

void ArFile_Write_Header(Archive_File_t* p_ar_file);

void Ar_Append_Directory(Archive_t* master_ar);

void Ar_Write_Master(Archive_t* master_ar, Archive_File_t* new_ar_file);

#endif /* ASSIGNMENT_02_INCLUDE_ARCHIVE */
