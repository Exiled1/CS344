
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

#define PIPE_COND 0
#define PARSE_COND 1

typedef int** fd_list_t;
typedef int* pid_list_t;
// char* g_input_line;
char g_prev_word[LINE_SIZE] = ""; // fite me.

pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

//int g_word_count = 1; // For counting the shit through semaphores using IPC.


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
        
        if(pipe(process->sort_pipes[cur_pipe]) == PIPE_FAILURE){
            fprintf(stderr, "Pipe #%d failed to open: [Errno = %d, errstr = %s]\n", cur_pipe,errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
        if(pipe(process->parse_pipes[cur_pipe]) == PIPE_FAILURE){
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
                // I might want to close the children...
                execlp("sort", "sort", (char*) NULL);
                fprintf(stderr, "Fork failed to open in child (case 0): [Errno = %d, errstr = %s]\n",errno, strerror(errno));
                exit(EXIT_FAILURE);
                break;
            default:
                close(process->parse_pipes[cur_pipe][STDIN_FILENO]);
                close(process->sort_pipes[cur_pipe][STDOUT_FILENO]);
                process->pid_list[cur_pipe] = spawn_pid;
                break;
        }
    }
    #ifdef THREAD
    pthread_cond_signal(&process->conditions[PIPE_COND]);
    #endif
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
    FILE** stream_buffers = malloc(sizeof(FILE*) * process->task_num); // Create enough buffers to satisfy the user designation.

    // Initialize all of the pipes and create file buffers.
    // Lock the thread because we're wanting to check if the pipes have been initalized.
    #ifdef THREAD
    pthread_mutex_lock(&g_lock);
    // Wait untill the buffers have been initialized before reading data.
    while(process->parse_pipes[process->task_num - 1][1] == PIPE_FAILURE){
        pthread_cond_wait(&process->conditions[PIPE_COND], &g_lock);
    }
    #endif

    for (cur_pipe = 0; cur_pipe < process->task_num; cur_pipe++) {
        // Open writing permissions to write to the file buffers.
        stream_buffers[cur_pipe] = fdopen(process->parse_pipes[cur_pipe][1], "w"); 
        if(stream_buffers[cur_pipe] == NULL){
            fprintf(stderr, "Failed to open stream # %d: [Errno = %d, errstr = %s]\n", cur_pipe, errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    #ifdef THREAD
    pthread_mutex_unlock(&g_lock);
    #endif
    // ! Turn parsed lines into tokenized input.
    // TODO: Make this a split line function.
    // TODO: Make temp_list into a temp_word
    // TODO: Make word_list either an actual list of words, or word_buffer.
    // Fill the line buffers with input stream data
    // get line data from stdin using fgets til its null.
    int word_count = 0;
    printf("Process # = %d\n", process->task_num);
    while (READLINE_STDIN(line_buff, LINE_SIZE)) {
        
        // * Convert the characters in the buffer into lowercase for case insensitive, and clear any non alphabetic characters.
        
        string_sanitize(line_buff);
        // ? Afterwards, we can tokenize the input by spaces.

        TOKENIZE_INTO(word_list, line_buff); // Macro to tokenize a line into a word list.
        // dest <- src btw ^

        while(*word_list){ // while we have a word array. 
            if(word_count == process->task_num){
                word_count = 0;
            }
            #ifdef DEBUG
            //printf("buffer #%d: %s\n", word_count ,*word_list);
            #endif
            fprintf(stream_buffers[word_count], "%s\n", *word_list++);
            
            word_count++;
        }
    }

    // TODO: Out of the while loops, data has been buffered, time to close the stream!!

    for(int i = 0; i < process->task_num; i++){
    
        fclose(stream_buffers[i]);
    }
    
    #ifdef THREAD
    pthread_cond_signal(&process->conditions[PARSE_COND]);
    #endif
}

int lexic_min_at(int buffer_num, char** buffer_heaps){
    // Find the index of the smallest lexicographical word in a list, this will help with a merge.
    int lmin_index = -2; // -1 is gonna be an error.
    int counter;
    for (int i = 0; i < buffer_num; i++) {
        if(buffer_heaps[i] != NULL){
            lmin_index = i;
            break;
        }
    }
    
    if(lmin_index == -2){
        return -2;
    }

    // Find the lowest lexi word.
    for(int i = 0; i < buffer_num; i++){
        if(buffer_heaps[i] == NULL){
            continue;
        }
        if(strcmp(buffer_heaps[i], buffer_heaps[lmin_index]) < 0){
            lmin_index = i;
        }
    }

    return lmin_index;
}

/**
 * @brief Given a sorted list of words, print the unique words in the list.
 * 
 */
void silencer(Uniq_proc_t* process){
    FILE** sort_data_buf = malloc(sizeof(FILE*) * process->task_num); // Create enough sort buffers to fit all of the tasks for merge later.
    char** word_arr = malloc(sizeof(char*) * process->task_num); // create a temporary buffer for the K buffers.
    int buffers_read = 0;
    // open the sort buffers and read from them
    #ifdef THREAD
    pthread_mutex_lock(&g_lock);
    pthread_cond_wait(&process->conditions[PARSE_COND], &g_lock);
    #endif
    for (int cur_pipe = 0; cur_pipe < process->task_num; cur_pipe++) {
        word_arr[cur_pipe] = malloc(sizeof(char) * LINE_SIZE); // Holds a 100 length word per buffer. 
        // ! ^ This doesn't handle any word or series of characters longer than 100 ascii characters
        sort_data_buf[cur_pipe] = fdopen(process->sort_pipes[cur_pipe][STDIN_FILENO], "r"); // Read from the current pipe, idk why it's the stdin tho.
        if(fgets(word_arr[cur_pipe], LINE_SIZE, sort_data_buf[cur_pipe]) == NULL){ 
            // When we've read the whole stream, end the thing with a null.
            word_arr[cur_pipe] = NULL; 
            printf("End of stream\n");
        }
        printf("%d word: %s\n", cur_pipe, word_arr[cur_pipe]);
    }
    #ifdef THREAD
    pthread_mutex_unlock(&g_lock);
    #endif  
    


    // get the index of the lexicographically smallest word out of the K min-heaps (sorted buffers)



    // Get 

}

void* thread_sort_caller(Uniq_proc_t* process){ 
  
    sort_pipe_init(process);
    return NULL;
}
void* thread_parse_caller(Uniq_proc_t* process){
    parser(process);
    return NULL;
}

void* thread_silence_caller(Uniq_proc_t* process){
    silencer(process);
    return NULL;
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
    process->task_num = task_num;
    printf("Task Num: %d\n", process->task_num);

    #ifdef THREAD

    printf("Thread code is badass raaaawr\n");

    #endif

    //int** input_fds; // list of the input file descriptors we'll be using for pipes.
    //int** sorting_fds; // This is going to be for how we handle the pipe I/O

    //pid_t spawnpid; // Get a spawning id for the process sort/merge.

    // Dynamically created list of the number of tasks we're going to have.
    process->parse_pipes = malloc(sizeof(int*) * task_num); 
    process->sort_pipes  = malloc(sizeof(int*) * task_num); 
    process->pid_list    = malloc(sizeof(pid_t) * task_num);
    
    // Loop for initializing the process pipes and creating enough space for the pipe() in/out space.
    for (int i = 0; i < process->task_num; i++)
    {
        process->parse_pipes[i] = malloc(sizeof(int) * PIPE_IO_SIZE); // Create enough space for the in/out file descriptors created by pipe.
        process->sort_pipes[i] = malloc(sizeof(int) * PIPE_IO_SIZE); // ^^ 
    }


    process->parse_pipes[process->task_num - 1][1] = PIPE_FAILURE;
    process->sort_pipes[process->task_num - 1][1] = PIPE_FAILURE;

    
    // FUCKING INITALIZE THE PIPES, I'VE TOLD THIS TO EVERYONE AND DIDNT FOLLOW MY OWN ADVICE.
    #ifdef PROCESS
    sort_pipe_init(process);
    // Parse input
    parser(process);
    // Set up piping to the sort process.
    silencer(process);
    #endif


    // (void*(*)(void*))
    // void* = return type
    // (*) = saying its a function
    // (void*) = casting the parameter types.

    #ifdef THREAD
    pthread_t threads[3];
    pthread_create(&threads[0], NULL, (void*(*)(void*))thread_sort_caller, process);
    pthread_create(&threads[1], NULL, (void*(*)(void*))thread_parse_caller, process);
    pthread_create(&threads[2], NULL, (void*(*)(void*))thread_silence_caller, process);
    
    for (size_t i = 0; i < 3; i++)
    {
        pthread_join(threads[i], NULL);
    }
    #endif THREAD

    //pthread_t tiddy;
    //pthread_create(&tiddy, NULL, thread_silence_caller, process);
    // Merge all of the tasks together.
    
    // Close the input/sort pipes

    // Free all the memory that you allocated
	for (int i = 0; i < task_num; i++) {
			wait(NULL);
	}

    return 0;
}
