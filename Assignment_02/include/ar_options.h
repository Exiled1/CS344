#ifndef ASSIGNMENT_02_INCLUDE_AR_OPTIONS
#define ASSIGNMENT_02_INCLUDE_AR_OPTIONS

#include <argp.h>

// Forward declaration of typedefs.
typedef struct argp_state Arg_state;
typedef struct arguments_t Arguments_t;
typedef struct argp_option Argp_option;

// Static options preserve their value after being out of scope. Usually I'd use pointers for this but there should only ever be one copy of this.
//? Format:
//? const char*, int, const char*, int, const char*, int
//? {"name", key, ?args, flags, *doc, group}

static struct argp_option options[] = {
    {"append", "q", 0, 0, "Quickly append files to an archive."},
    {"extract", "x", 0, 0, "Extract named files from an archive."},
    {"concise", "t", 0, 0, "Print a CONCISE table of contents of the archive."},
    {"verbose", "v", 0, 0, "Print a VERBOSE table of contents of the archive."},
    {"delete", "d", 0, 0, "Delete named files from an archive."},
    {"append-all", "A", 0, 0, "Quickly append all 'regular' files in the current directory except the archive itself and binary files."},
};

typedef struct arguments_t{
    char* arg1; // 
    char* archive_name; // 
    char** files;
}Arguments_t;

static error_t parse_opt (int key, char *arg, struct argp_state *state);

#endif /* ASSIGNMENT_02_INCLUDE_AR_OPTIONS */
