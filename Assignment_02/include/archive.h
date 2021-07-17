#ifndef ASSIGNMENT_02_HEADERS_ARCHIVE
#define ASSIGNMENT_02_HEADERS_ARCHIVE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <utime.h>
#include <sys/stat.h>

#define NAME_SIZE 32


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
    bool option_q = false; // Keep track of option -q
    bool option_x = false; // Keep track of option -x
    bool option_t = false; // Keep track of option -t
    bool option_v = false; // Keep track of option -v
    bool option_d = false; // Keep track of option -d
    bool option_A = false; // Keep track of option -A
    bool option_w = false; // Keep track of option -w

    char archive_name[NAME_SIZE]; // Name of the archive.
    int total_files; // Total files in the archive.
    FILE* master_archive; // The file where all of the archive files are written to.
    Archive_File_t* head; // keeps track of the archive files, malloc.
}Archive_t;

#endif /* ASSIGNMENT_02_HEADERS_ARCHIVE */
