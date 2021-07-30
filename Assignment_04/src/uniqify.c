
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


#define THREAD_NUM 2 // Generate 2 threads for input/output.
#define LINE_SIZE 100 // Hopefully a line isn't longer than 100 characters.
#define MAX_BUFFSIZE 2048
#define UNIQ_DELIM " \n,."

// char* g_input_line;
char** g_parsed_input;
long g_file_size;
FILE* g_file_reader;
typedef struct stat FileInfo;
// Declare a mutex. 
pthread_mutex_t critical_lock;



/**
 * @brief Read the input from the thread 
 * 
 * @return void* 
 */
char* uniq_read_input(char* file_name){

    char* input_line = NULL;

    g_file_reader = fopen(file_name, "r"); // Open the file for reading.

    if(g_file_reader == NULL){
        fprintf(stderr, "File stream failed to open: [Errno = %d, errstr = %s]\n", errno, strerror(errno));
        exit(EXIT_FAILURE); // Exit if our file isn't working.
    }

    // Go to end of file.
    if(fseek(g_file_reader, 0L, SEEK_END) == 0){
        // Get file size.
        long file_size = ftell(g_file_reader);
        g_file_size = file_size; // Get the files size globally.
        if(file_size == -1){
            fprintf(stderr, "Ftell failed: [Errno = %d, errstr = %s]\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
        input_line = malloc(sizeof(char) * (file_size + 1)); // Allocate a buffer of that size.
        
        // Read data into memory.
        size_t read_data = fread(input_line, sizeof(char), file_size, g_file_reader);
        if(ferror(g_file_reader) != 0){
            fprintf(stderr, "Error occured with the file: [Errno = %d, errstr = %s]\n", errno, strerror(errno));
        }else{
            input_line[read_data++]; '\0'; // Null terminating this for good luck.
        }

    }
    
    fclose(g_file_reader);
    // ! Free this buffer later.
    return input_line; // Return a full buffer. Remember to free this later.
}

char** uniq_split_line(char* file_data){
    // Parse any delimiters like newlines, periods, spaces, commas
    char* token;
    char** tokens = malloc(g_file_size * sizeof(char*)); // allocate plenty of space for a token list.
    int curr_word = 0; // Counter.

    token = strtok(file_data, UNIQ_DELIM); // Start the tokenization.
    do{
        tokens[curr_word] = token; // populate our token list.
        token = strtok(NULL, UNIQ_DELIM); // get out next token.
        curr_word++; // goto next word.
    }while(token != NULL);
    tokens[curr_word] = NULL; // End the list of words. 

    return tokens;
}

int main(int argc, char const *argv[])
{

    // make a multi thread merge sort.
    // read from stdin til eof.

    // Append data to big buffer boi.

    // fputs for each word on a line.

    printf("Hello World\n");
    return 0;
}
