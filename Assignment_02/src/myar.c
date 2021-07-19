#include "archive.h"

int main(int argc, char const *argv[])
{
    // TODO: Get the files from the command line
    // TODO: Get the flags parsed
    // TODO: 
    // TODO:
    // TODO:
    // TODO:
    // TODO:
    Archive_Help(); // Print usage

    if(argc < 3){ // 
        printf("myar: You must specify one of the '-qxtvdA' options.\n");
        printf("Try 'myar -h' for more detailed usage information.\n");
        exit(-2); 
    }
    
    return 0;
}
