
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h> 
#include <sys/stat.h>
#include <errno.h>
#include <string.h>  
#include <sys/wait.h>
#include <fcntl.h>   
#include <stdbool.h> 
#include <signal.h>  


#define LINE_SIZE 100 // Hopefully a line isn't longer than 100 characters.
#define MAX_BUFFSIZE 2048


// char* g_input_line;
char g_prev_word[LINE_SIZE] = "";

void parser(){ // create a parser that parses through a file and takes a certain amount of words in.

}

/**
 * @brief Read the input from the thread 
 * 
 * @return void* 
 */


int main(int argc, char const *argv[])
{

    // make a multi thread merge sort.
    // read from stdin til eof.

    // Append data to big buffer boi.


    //while ( *words ) printf( "%s\n", *words++ );
    
    return 0;
}
