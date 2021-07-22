#include "archive.h"
#include "ar_options.h"



static char doc[] = "'myar' saves any number of files into an archive file, it's also able to extract files from a 'myar' archive file.";
static char args_doc[]="[FILENAME]"; // No standard args?


static struct argp_option options[] = {
    {"append",  'q', 0, 0, "Quickly append files to an archive."},
    {"extract", 'x', 0, 0, "Extract named files from an archive."},
    {"concise", 't', 0, 0, "Print a CONCISE table of contents of the archive."},
    {"verbose", 'v', 0, 0, "Print a VERBOSE table of contents of the archive."},
    {"delete",  'd', 0, 0, "Delete named files from an archive."},
    {"append-all", 'A', 0, 0, "Quickly append all 'regular' files in the current directory except the archive itself and binary files."},
};

struct argp argp = {options, parse_opt, args_doc, doc, 0, 0};
error_t parse_opt (int key, char *arg, Arg_state *state);

int main(int argc, char **argv)
{
    // TODO: Get the files from the command line    ✔
    // TODO: Get the flags parsed                   ✔
    // TODO: 
    // TODO:
    // TODO:
    // TODO:
    // TODO:
    Arguments_t arguments; // Make an empty argument list.
    Archive_t master_archive; // Make a master archive 
    
    

    if(argc < 3){ // 
        printf("myar: You must specify one of the '-qxtvdA' options.\n");
        printf("Try 'myar -h' for more detailed usage information.\n");
        Archive_Help(); // Print usage
        exit(-2); 
    }

    Arg_Set_Defaults(&arguments); // Set the argument type to its default values.
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    printf("Amount of files passed: %d\n", arguments.file_amt);
    printf("Files: ");
    for(int i = 0; i < arguments.file_amt; i++){
        printf("%s ", arguments.files[i]);
    }
    printf("\n"); 
    // For debug purposes.
    // printf("Append: %d\n"       , arguments.option_q);
    // printf("Extract: %d\n"      , arguments.option_x);
    // printf("Concise: %d\n"      , arguments.option_t);
    // printf("Verbose: %d\n"      , arguments.option_v);
    // printf("Delete: %d\n"       , arguments.option_d);
    // printf("Dir Append: %d\n"   , arguments.option_A);
    // printf("Timeout (w): %d\n"  , arguments.option_w);
    
    // Create the master archive here ---------------
    // * Set the name of the archive to the first CLI argument since an archive file should always be specified.
    master_archive = Archive_Create(arguments.files[0]);
    Arg_AR_Transfer(&arguments, &master_archive);
    //printf("Thing name: %s", master_archive.archive_name);
    // ? WE FINALLY GET TO THE ACTUAL PROGRAM SHIT.

    // if (Archive_No_Flags(&master_archive) == true) // No flags have been set in our archive.
    // {
    //     printf("Please provide flags for your archive.\n");
    //     Archive_Help();
    //     exit(-1);
    // }
    
    //TODO: Define the behavior of the individual flags.
    if(master_archive.ar_flags.option_q == true){ // Quick append
        
        printf("Q flag has been triggered.\n");
        master_archive.master_archive = fopen(arguments.files[0], "w");
        fputs(ARMAG,master_archive.master_archive); // Only works if stream has been opened.
        fclose(master_archive.master_archive); // close stream
        for (int current_file = 1; current_file < master_archive.total_files; current_file++)
        {
            printf("Current file Name: %s\n",  arguments.files[current_file]);
            printf("Current file #: %d\n",  current_file);

            Archive_Append_File(&master_archive, arguments.files[current_file]); // append these files. 
            
        }
        master_archive = Archive_Full_Write(&master_archive, arguments.files[0]);

    }else if(master_archive.ar_flags.option_x == true){ // Extraction

    }else if(master_archive.ar_flags.option_t == true){ // Concise
        // master_archive.master_archive = fopen(arguments.files[0], "r"); // open the thing.
        long files = ar_get_file_num(&master_archive, arguments.files[0]);
        printf("Number of files in archive: %ld\n", files);

    }else if(master_archive.ar_flags.option_v == true){ // Verbose
        printf("Rawr xd\n");
        Archive_t new_archive = Archive_Create(arguments.files[0]);
        long files = ar_get_file_num(&new_archive, arguments.files[0]); // gets amount of files in an archive file.
        new_archive.total_files = files;
        Archive_Retrieve(&new_archive, arguments.files[0]);

    }else if(master_archive.ar_flags.option_d == true){ // Delete
        Archive_t new_archive = Archive_Create(arguments.files[0]);
        long files = ar_get_file_num(&master_archive, arguments.files[0]); // gets amount of files in an archive file.
        new_archive.total_files = files;
  
        Archive_Retrieve(&new_archive, arguments.files[0]);

        Arg_AR_Transfer(&arguments, &new_archive);

        printf("Del flag triggered.\n");

        // ? Open
        //master_archive.master_archive = fopen(arguments.files[0], "r");
        // fputs(ARMAG,master_archive.master_archive); // Only works if stream has been opened.
        // fclose(master_archive.master_archive);
        // ? Close

        for(int current_file = 1; current_file < master_archive.total_files; current_file++){
            printf("Current file #: %s\n",  arguments.files[current_file]);
            Archive_Delete_File(&new_archive, arguments.files[current_file]);
        }
        new_archive = Archive_Full_Write(&new_archive, arguments.files[0]);

    }else if(master_archive.ar_flags.option_A == true){ // Dir append
        Archive_Append_Directory(&master_archive);
    }else if(master_archive.ar_flags.option_w == true){ // Extra cred.

    }
    //fclose(master_archive.master_archive);
    return 0;
}
