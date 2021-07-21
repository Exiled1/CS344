#include "ar_options.h"
#include "archive.h"
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <stdbool.h>


// myar -q archive.ar foo.txt bar.txt

error_t parse_opt (int key, char *arg, Arg_state *state){
    Arguments_t *arguments = state->input;
    switch(key){
        case 'q':
            arguments->option_q = true;
            break;
        case 'x':
            printf("Supposed to extract.\n");
            arguments->option_x = true;
            break;
        case 't':
            arguments->option_t = true;
            break;
        case 'v':
            arguments->option_v = true;
            break;
        case 'd':
            arguments->option_d = true;
            break;
        case 'A':
            arguments->option_A = true;
            break;
        case 'w':
            arguments->option_w = true;
            break;
        
        case ARGP_KEY_ARGS:
            {
                arguments->files = &state->argv[state->next];
                state->next = state->argc;
            }
            break;
        case ARGP_KEY_NO_ARGS:
            {
                printf("Please specify an archive file.\n");
                argp_usage(state);
                break;
            }
        case ARGP_KEY_END:
            {
                
                printf("Reached the end of the options, printing anything relevant.\n");
                arguments->file_amt = state->arg_num;
            }
            break;
            // Reached last key, 
        
        default:
            return ARGP_ERR_UNKNOWN;
    }
    
    return 0;

}

void Arg_Set_Defaults(Arguments_t* arguments){
    arguments->option_q = false;
    arguments->option_x = false;
    arguments->option_t = false;
    arguments->option_v = false;
    arguments->option_d = false;
    arguments->option_A = false;
    arguments->option_w = false;
}

void Arg_AR_Transfer(Arguments_t* arguments, Archive_t* master_ar){
    // Copying over the parsed argument flags over to the master struct.
    master_ar->ar_flags.option_q = arguments->option_q;
    master_ar->ar_flags.option_x = arguments->option_x;
    master_ar->ar_flags.option_t = arguments->option_t;
    master_ar->ar_flags.option_v = arguments->option_v;
    master_ar->ar_flags.option_d = arguments->option_d;
    master_ar->ar_flags.option_A = arguments->option_A;
    master_ar->ar_flags.option_w = arguments->option_w;
    // 
    // memcpy(master_ar->archive_name, arguments->files[0], NAME_SIZE); // Copy over the archive name, since it should always be the first argument in the array.
    master_ar->total_files = arguments->file_amt;

}

