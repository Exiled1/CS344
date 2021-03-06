#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>
#include "../include/msort.h"

#define LINE_SIZE 400 // Hopefully a line isn't longer than 100 characters.
#define MAX_BUFFSIZE 2048
#define PIPE_IO_SIZE 2
#define PIPE_OUT 1 // Stdout
#define PIPE_IN 0  // Stdin
#define SUCCESS 0
#define FAILURE 1
#define TOKEN_DELIMS " "
#define PIPE_FAILURE -1

#define PIPE_COND 0
#define PARSE_COND 1

typedef int **fd_list_t;
typedef int *pid_list_t;

pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;


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

    //clock_t start, end; // For timing.
    
    Uniq_proc_t *process = malloc(sizeof(Uniq_proc_t));
    
    process->task_num = task_num;

    process->parse_pipes = malloc(sizeof(int *) * task_num);
    process->sort_pipes = malloc(sizeof(int *) * task_num);
    process->pid_list = malloc(sizeof(pid_t) * task_num);

    // Loop for initializing the process pipes and creating enough space for the pipe() in/out space.
    for (int i = 0; i < process->task_num; i++)
    {
        process->parse_pipes[i] = malloc(sizeof(int) * PIPE_IO_SIZE); // Create enough space for the in/out file descriptors created by pipe.
        process->sort_pipes[i] = malloc(sizeof(int) * PIPE_IO_SIZE);  // ^^
    }

    process->parse_pipes[process->task_num - 1][1] = PIPE_FAILURE;
    process->sort_pipes[process->task_num - 1][1] = PIPE_FAILURE;

// FUCKING INITALIZE THE PIPES, I'VE TOLD THIS TO EVERYONE AND DIDNT FOLLOW MY OWN ADVICE.
#ifdef PROCESS
    //start = clock();
    
    sort_pipe_init(process);
    // Parse input
    parser(process);
    // Set up piping to the sort process.
    fork_silencer(process);
    //end = clock();
#endif

    // (void*(*)(void*))
    // void* = return type
    // (*) = saying its a function
    // (void*) = casting the parameter types.

#ifdef THREAD

    pthread_t threads[3];
    pthread_create(&threads[0], NULL, (void* (*)(void *))thread_sort_caller, process);
    
    pthread_create(&threads[1], NULL, (void* (*)(void *))thread_parse_caller, process);
    
    pthread_create(&threads[2], NULL, (void* (*)(void *))thread_silence_caller, process);
    

    for (size_t i = 0; i < 3; i++)
    {
        pthread_join(threads[i], NULL);
    }

#endif

    // Close the input/sort pipes
    // Free all the memory that you allocated
    for (int i = 0; i < task_num; i++)
    {
       wait(NULL);
    }
    //end = clock();
    //double time_taken = (double)(end - start) / (double)(CLOCKS_PER_SEC);
    //printf("Program took: %f seconds", time_taken);
    return 0;
}

/**
 * @brief Initializes all of the pipe lists and sets the sorting lists to output to the sort process. 
 * 
 * @param task_num 
 * @param input_fds 
 * @param sorting_fds 
 */
void sort_pipe_init(Uniq_proc_t *process)
{
    // TODO: Create the pipes to the sort process. Done.

    pid_t spawn_pid;
    // For however many tasks we need to spawn, create a new pipe that reads from stdin and writes to stdout

    for (int cur_pipe = 0; cur_pipe < process->task_num; cur_pipe++)
    {

        if (pipe(process->sort_pipes[cur_pipe]) == PIPE_FAILURE)
        {
            fprintf(stderr, "Pipe #%d failed to open: [Errno = %d, errstr = %s]\n", cur_pipe, errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (pipe(process->parse_pipes[cur_pipe]) == PIPE_FAILURE)
        {
            fprintf(stderr, "Pipe #%d failed to open: [Errno = %d, errstr = %s]\n", cur_pipe, errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        switch (spawn_pid = fork())
        {
        case -1: // Error in the fork.
            fprintf(stderr, "Fork failed to open, (case -1): [Errno = %d, errstr = %s]\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        case 0: // Child spawned
            /*
                 This case should close the writing from the input parsing, since we only need to read data from stdin. And we should close the read for the sorting, since we're only going to be writing the data out to 
                */

            dup2(process->parse_pipes[cur_pipe][STDIN_FILENO], STDIN_FILENO);
            dup2(process->sort_pipes[cur_pipe][STDOUT_FILENO], STDOUT_FILENO);
            // Initialize the pipes, then close the parser for later use.
            close_pipes_array(process->parse_pipes, cur_pipe);
            close_pipes_array(process->sort_pipes, cur_pipe);

            // TODO: I might want to close the children...
            execlp("sort", "sort", (char *)NULL);
            fprintf(stderr, "Fork failed to open in child (case 0): [Errno = %d, errstr = %s]\n", errno, strerror(errno));
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
void string_sanitize(char *input)
{

    for (int cur_char = 0; cur_char < strlen(input); cur_char++)
    { //
        if (isalpha(input[cur_char]) != SUCCESS)
        {                                               // If it finds an alphabetic character:
            input[cur_char] = tolower(input[cur_char]); // Make that letter lowercase.
        }
        else
        {
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
char **string_tokenize(char *input)
{
    char *token;
    char **tokens = malloc(LINE_SIZE * sizeof(char *));
    // Create a list of words.

    int cur_word = 0;
    token = strtok(input, TOKEN_DELIMS); // Seperate by spaces.

    do
    {
        tokens[cur_word] = token;           // place token into buffer.
        token = strtok(NULL, TOKEN_DELIMS); // get next token.
        cur_word++;                         // next word
    } while (token != NULL);
    tokens[cur_word] = NULL;

    return tokens;
}

/**
 * @brief Does not need to work with different process/thread types. Given the number of tasks, goes round-robin parsing 
 * until the input has been fully read in.
 * 
 */
void parser(Uniq_proc_t *process)
{
    // TODO: Split this up into helper methods to do everything
    // create a parser that parses through a file and takes a certain amount of words in.
    char line_buff[LINE_SIZE]; // Create a general input buffer.
    char **word_list;          // Maybe allocate this if we sending it.
    int cur_pipe = 0;

    // TODO: Allocate a list of file buffers for the piping.
    FILE **stream_buffers = malloc(sizeof(FILE *) * process->task_num); // Create enough buffers to satisfy the user designation.

// Initialize all of the pipes and create file buffers.
// Lock the thread because we're wanting to check if the pipes have been initalized.
#ifdef THREAD
    pthread_mutex_lock(&g_lock);
    // Wait untill the buffers have been initialized before reading data.
    while (process->parse_pipes[process->task_num - 1][1] == PIPE_FAILURE)
    {
        pthread_cond_wait(&process->conditions[PIPE_COND], &g_lock);
    }
#endif

    for (cur_pipe = 0; cur_pipe < process->task_num; cur_pipe++)
    {
        // Open writing permissions to write to the file buffers.
        stream_buffers[cur_pipe] = fdopen(process->parse_pipes[cur_pipe][1], "w");
        if (stream_buffers[cur_pipe] == NULL)
        {
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
    while (READLINE_STDIN(line_buff, LINE_SIZE))
    {

        // * Convert the characters in the buffer into lowercase for case insensitive, and clear any non alphabetic characters.

        string_sanitize(line_buff);
        // ? Afterwards, we can tokenize the input by spaces.

        TOKENIZE_INTO(word_list, line_buff); // Macro to tokenize a line into a word list.
        // dest <- src btw ^

        while (*word_list)
        { // while we have a word array.
            if (word_count == process->task_num)
            {
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

    for (int i = 0; i < process->task_num; i++)
    {

        fclose(stream_buffers[i]);
    }

#ifdef THREAD
    pthread_cond_signal(&process->conditions[PARSE_COND]);
#endif
}

int lexic_min_at(int buffer_num, char **buffer_heaps)
{
    // Find the index of the smallest lexicographical word in a list, this will help with a merge.
    int lmin_index = -2; // -1 is gonna be an error.

    for (int i = 0; i < buffer_num; i++)
    {
        if (buffer_heaps[i] != NULL)
        {
            lmin_index = i;
            break;
        }
    }

    if (lmin_index == -2)
    {
        return -2;
    }

    // Find the lowest lexi word.
    for (int i = 0; i < buffer_num; i++)
    {
        if (buffer_heaps[i] == NULL)
        {
            continue;
        }
        if (strcmp(buffer_heaps[i], buffer_heaps[lmin_index]) < 0)
        {
            lmin_index = i;
        }
    }

    return lmin_index;
}

/**
 * @brief Given a sorted list of words, print the unique words in the list.
 * 
 */
void silencer(Uniq_proc_t *process)
{
    FILE **sort_data_buf = malloc(sizeof(FILE *) * process->task_num); // Create enough sort buffers to fit all of the tasks for merge later.
    char **word_arr = malloc(sizeof(char *) * process->task_num);      // create a temporary buffer for the K buffers.
    int buffers_read = 0;
    int lexi_small_index = -2;
    word_track_t word_tracker; // If need to let out of here, malloc :shrug:
// open the sort buffers and read from them
#ifdef THREAD
    pthread_mutex_lock(&g_lock);
    pthread_cond_wait(&process->conditions[PARSE_COND], &g_lock);
#endif
    for (int cur_pipe = 0; cur_pipe < process->task_num; cur_pipe++)
    {
        word_arr[cur_pipe] = malloc(sizeof(char) * LINE_SIZE); // Holds a 100 length word per buffer.
        // ! ^ This doesn't handle any word or series of characters longer than 100 ascii characters
        sort_data_buf[cur_pipe] = fdopen(process->sort_pipes[cur_pipe][STDIN_FILENO], "r"); // Read from the current pipe, idk why it's the stdin tho.
        if (fgets(word_arr[cur_pipe], LINE_SIZE, sort_data_buf[cur_pipe]) == NULL)
        {
            // When we've read the whole stream, end the thing with a null.
            word_arr[cur_pipe] = NULL;
            #ifdef DEBUG
            printf("End of stream\n");
            #endif
        }
#ifdef DEBUG
        printf("%d word: %s\n", cur_pipe, word_arr[cur_pipe]);
#endif
    }

    lexi_small_index = lexic_min_at(process->task_num, word_arr); // Finds index of lexic lowest word.

    if (lexi_small_index == -2)
    {
        fprintf(stderr, "None of the buffers had any data.\n");
        exit(EXIT_FAILURE);
    }

    // TODO: Do a thing for word count inside of a struct.
    strncpy(word_tracker.word, word_arr[lexi_small_index], LINE_SIZE); // Place the word into the word tracker.
    word_tracker.occurrences = 1;                                      // First word encountered.

    // while we're getting data from all the buffers til the end of the buffers:
    while (buffers_read < process->task_num)
    {
        if (fgets(word_arr[lexi_small_index], LINE_SIZE, sort_data_buf[lexi_small_index]) == NULL)
        {
            word_arr[lexi_small_index] = NULL; // Idek if I need to null it... Probably not.
            buffers_read++;
        }
        lexi_small_index = lexic_min_at(process->task_num, word_arr);
        if (lexi_small_index == -2)
        {
            // Word arrays are all empty.
            break;
        }
        // Compare current word and the next searched word, if it's the same word increment the count.
        if (strcmp(word_tracker.word, word_arr[lexi_small_index]) == 0)
        {
#ifdef DEBUG
            printf("First word: %s\nSecond word: %s", word_tracker.word, word_arr[lexi_small_index]);
#endif
            word_tracker.occurrences++;
        }
        else
        {
            // If the word is different, print the word and acquire the new word from the min heaps.
            printf("%7d %s", word_tracker.occurrences, word_tracker.word);
            strncpy(word_tracker.word, word_arr[lexi_small_index], LINE_SIZE); // Place the word into the word tracker.
            word_tracker.occurrences = 1;
        }
    }
    // print final word
    printf("%7d %s", word_tracker.occurrences, word_tracker.word);

#ifdef THREAD
    pthread_mutex_unlock(&g_lock);
#endif

    // get the index of the lexicographically smallest word out of the K min-heaps (sorted buffers)

    for (int i = 0; i < process->task_num; i++)
    {
        fclose(sort_data_buf[i]);
    }

    // Get
}

void *thread_sort_caller(Uniq_proc_t *process)
{
    sort_pipe_init(process);
    return NULL;
}

void *thread_parse_caller(Uniq_proc_t *process)
{
    parser(process);
    return NULL;
}

void *thread_silence_caller(Uniq_proc_t *process)
{
    silencer(process);
    return NULL;
}

void close_pipes_array(int **pipe_array, int end)
{
    int i, j;
    for (i = 0; i <= end; i++)
    {
        for (j = 0; j < 2; j++)
        {
            /* I recieved this from office hours and also some stack overflow */
            close(pipe_array[i][j]);
        }
    }
}

void fork_silencer(Uniq_proc_t* process){

    pid_t spawnpid;
    spawnpid = fork();

    switch (spawnpid) {
    case -1:
        fprintf(stderr, "Fork failed to open in fork_silencer: [Errno = %d, errstr = %s]\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
        break;
    
    case 0:
        silencer(process);
        break;

    default:
        wait(&spawnpid);
        break;
    }
}