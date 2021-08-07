#define _POSIX_C_SOURCE 2
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
#include <ctype.h>
#include "../include/uniqify.h"


#define LINE_SIZE 100 // Hopefully a line isn't longer than 100 characters.
#define MAX_BUFFSIZE 2048
#define PIPE_IO_SIZE 2
#define PIPE_OUT 1 // Stdout
#define PIPE_IN 0 // stdin
#define SUCCESS 0
#define FAILURE 1
#define TOKEN_DELIMS " "
#define PIPE_FAILURE -1
typedef int** fd_list_t;
typedef int* pid_list_t;
// char* g_input_line;
char g_prev_word[LINE_SIZE] = ""; // fite me.
int g_word_count = 1; // For counting the shit through semaphores using IPC.


/**
 * @brief Initializes all of the pipe lists and sets the sorting lists to output to the sort process. 
 * 
 * @param task_num 
 * @param input_fds 
 * @param sorting_fds 
 */
void sort_pipe_init(Uniq_proc_t* process){
    // TODO: Create the pipes to the sort process. Done.
    
    pid_t spawn_pid;
    // For however many tasks we need to spawn, create a new pipe that reads from stdin and writes to stdout
    for(int cur_pipe = 0; cur_pipe < process->task_num; cur_pipe++){

        if(pipe(process->parse_pipes[cur_pipe]) == PIPE_FAILURE || pipe(process->sort_pipes[cur_pipe]) == PIPE_FAILURE){
            fprintf(stderr, "Pipe #%d failed to open: [Errno = %d, errstr = %s]\n", cur_pipe,errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
        switch(spawn_pid = fork()){
            case -1: // Error in the fork.
                fprintf(stderr, "Fork failed to open, case -1: [Errno = %d, errstr = %s]\n",errno, strerror(errno));
                exit(EXIT_FAILURE);
            case 0: // Child spawned
                /*
                 This case should close the writing from the input parsing, since we only need to read data from stdin. And we should close the read for the sorting, since we're only going to be writing the data out to 
                */
                dup2(process->parse_pipes[cur_pipe][STDIN_FILENO], STDIN_FILENO);
                dup2(process->sort_pipes[cur_pipe][STDOUT_FILENO], STDOUT_FILENO);
                // Initialize the pipes, then close the parser for later use.
                close(process->parse_pipes[cur_pipe][STDIN_FILENO]);
                close(process->parse_pipes[cur_pipe][STDOUT_FILENO]);
                // Close the sort pipes and then afterwards run the exec for sort.
                close(process->sort_pipes[cur_pipe][STDIN_FILENO]);
                close(process->sort_pipes[cur_pipe][STDOUT_FILENO]);
                execlp("sort", "sort", (char*) NULL);
                fprintf(stderr, "Fork failed to open in child (case 0): [Errno = %d, errstr = %s]\n",errno, strerror(errno));
                exit(EXIT_FAILURE);
                break;
            default:
                close(process->parse_pipes[cur_pipe][STDIN_FILENO]);
                close(process->parse_pipes[cur_pipe][STDOUT_FILENO]);
                process->pid_list[cur_pipe] = spawn_pid;
                break;

        }
    }
}
/**
 * @brief Takes a string and removes non alphabetic characters from it (Doesn't work on unicode. I think?)
 * 
 * @param input 
 */
void string_sanitize(char* input){

    for(int cur_char = 0; cur_char < strlen(input); cur_char++){ // 
        if(isalpha(input[cur_char]) != SUCCESS){ // If it finds an alphabetic character:
            input[cur_char] = tolower(input[cur_char]); // Make that letter lowercase.
        }else{
            input[cur_char] = ' '; // otherwise, clear it from the buffer.
        }
    }
}
/**
 * @brief Tokenizes a line and returns a list of the words in the line. Assumes string_sanitize was executed beforehand.
 * 
 * @param input 
 * @return char** 
 */
char** string_tokenize(char* input){
    char* token;
    char** tokens = malloc(LINE_SIZE * sizeof(char*)); 
    // Create a list of words.

    int cur_word = 0;
    token = strtok(input, TOKEN_DELIMS); // Seperate by spaces.

    do{
        tokens[cur_word] = token; // place token into buffer.
        token = strtok(NULL, TOKEN_DELIMS); // get next token.
        cur_word++; // next word
    }while(token != NULL);
    tokens[cur_word] = NULL; 

    return tokens;
}

void pipe_send_data(Uniq_proc_t* process, char** word_list, FILE** stream_buffers, int counter){
    int i = 0;
    while(*word_list){
        fputs(word_list[count], stream_buffers[count])
    }
}

/**
 * @brief Does not need to work with different process/thread types. Given the number of tasks, goes round-robin parsing 
 * until the input has been fully read in.
 * 
 */
void parser(Uniq_proc_t* process){
    // TODO: Split this up into helper methods to do everything
    // create a parser that parses through a file and takes a certain amount of words in.
    char line_buff[LINE_SIZE]; // Create a general input buffer.
    char** word_list; // Maybe allocate this if we sending it.
    int cur_pipe = 0;

    // TODO: Allocate a list of file buffers for the piping.
    FILE** file_buffers = malloc(sizeof(FILE*) * process->task_num); // Create enough buffers to satisfy the user designation.

    // Initialize all of the pipes and create file buffers.

    for (cur_pipe = 0; cur_pipe < process->task_num; cur_pipe++) {

        // Open writing permissions to write to the file buffers.
        file_buffers[cur_pipe] = fdopen(process->parse_pipes[cur_pipe][PIPE_OUT], "w"); 
        if(file_buffers[cur_pipe] == NULL){
            fprintf(stderr, "Failed to open pipe # %d: [Errno = %d, errstr = %s]\n", cur_pipe, errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }


    
    // ! Turn parsed lines into tokenized input.
    // TODO: Make this a split line function.
    // TODO: Make temp_list into a temp_word
    // TODO: Make word_list either an actual list of words, or word_buffer.
    // Fill the line buffers with input stream data
    // get line data from stdin using fgets til its null.
    int word_count = 0;
    while (READLINE_STDIN(line_buff, LINE_SIZE)) {
        
        // * Convert the characters in the buffer into lowercase for case insensitive, and clear any non alphabetic characters.
        
        string_sanitize(line_buff);
        // ? Afterwards, we can tokenize the input by spaces.

        TOKENIZE_INTO(word_list, line_buff); // Macro to tokenize a line into a word list.
        // dest <- src btw ^

        PRINT_STR_LIST(word_list); // Macro to print the words in a string array to console.

        //pipe_send_data()

    }
    
    

}

/**
 * @brief Given a sorted list of words, print the unique words in the list.
 * 
 */
void silencer(char** word_list){
    
}

void process_sorting(){

}

void thread_sorting(){

}

void process_merging(){

}

void thread_merging(){

}

/**
 * @brief Read the input from the thread 
 * 
 * @return void* 
 */


int main(int argc, char const *argv[])
{

    /**
     * @brief So it seems that this is gonna be more interesting than I thought. Essentially 
     * it seems that we're going to have to make N pipes that the user gives us from the CMD line. 
     * 
     */
    int task_num = atoi(argv[1]); // get the number from input.

    //printf("Number of tasks: %d\n", task_num);
    Uniq_proc_t* process = malloc(sizeof(Uniq_proc_t));

    int** input_fds; // list of the input file descriptors we'll be using for pipes.
    int** sorting_fds; // This is going to be for how we handle the pipe I/O

    pid_t spawnpid; // Get a spawning id for the process sort/merge.

    // Dynamically created list of the number of tasks we're going to have.
    input_fds = calloc(sizeof(int*), task_num); 
    sorting_fds = calloc(sizeof(int*), task_num); 
    
    // Loop for initializing the process pipes and creating enough space for the pipe() in/out space.
    for (int i = 0; i < task_num; i++)
    {
        input_fds[i] = calloc(sizeof(int), PIPE_IO_SIZE); // Create enough space for the in/out file descriptors created by pipe.
        sorting_fds[i] = calloc(sizeof(int), PIPE_IO_SIZE); // ^^ 
    }
    
    // Parse input

    parser(process);

    // Set up piping to the sort process.

    // Merge all of the tasks together.

    // Close the input/sort pipes

    // Free all the memory that you allocated


    return 0;
}
