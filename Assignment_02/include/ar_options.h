#ifndef ASSIGNMENT_02_INCLUDE_AR_OPTIONS
#define ASSIGNMENT_02_INCLUDE_AR_OPTIONS
#include <stdbool.h>
#include <argp.h>
#include "archive.h"
// Forward declaration of typedefs.
typedef struct argp_state Arg_state;
typedef struct arguments_t Arguments_t;
typedef struct argp_option Argp_option;

// Static options preserve their value after being out of scope. Usually I'd use pointers for this but there should only ever be one copy of this.
//? Format:
//? const char*, int, const char*, int, const char*, int
//? {"name", key, ?args, flags, *doc, group}



//char args_doc[] = ""; // No standard args?

typedef struct arguments_t{
    char* arg1; // 
    char* archive_name; //
    int file_amt;
    char** files;
    bool option_q; // Keep track of option -q
    bool option_x; // Keep track of option -x
    bool option_t; // Keep track of option -t
    bool option_v; // Keep track of option -v
    bool option_d; // Keep track of option -d
    bool option_A; // Keep track of option -A
    bool option_w; // Keep track of option -w
}Arguments_t;

error_t parse_opt (int key, char *arg, Arg_state *state);
void Arg_Set_Defaults(Arguments_t* arguments);
void Arg_AR_Transfer(Arguments_t* arguments, Archive_t* master_ar);
#endif /* ASSIGNMENT_02_INCLUDE_AR_OPTIONS */
