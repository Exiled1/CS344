/**
 * 
 */

#include<stdio.h>   
#include<stdlib.h>  
#include<string.h>  
#include<unistd.h>  
#include<sys/wait.h>
#include<fcntl.h>   
#include<stdbool.h> 
#include<signal.h>  

#define PROMPT_CHAR ": "
#define SM_DELIMS " \n"
#define MAX_BUFFSIZE 2048 // max buffer for any command.
#define ARGMAX 512  // max args for a command.

int fg_pid = 0; // Global foreground process PID tracker.
void smsh_sigint_handler();
void shell_init();
char* smsh_read_line();
void check_buffer(int taken_size);
char** smsh_split_line(char* input_line);
int smsh_exec_cmd(char** args);

int main(int argc, char const *argv[])
{
    shell_init();
    return 0;
}


void shell_init(){
    char* input_str;
    char** args;
    int state; // state flag to keep track of the program
    int i = 0;
    do{
        printf(PROMPT_CHAR);
        input_str = smsh_read_line();
        args = smsh_split_line(input_str);
        state = smsh_exec_cmd(args);
        // printf("%s", input_str);
    }while(state != 1);
}
// Driver code ^^ 


/**
 * @brief Read the lines that come in, no parsey parsey.
 * 
 * @return char* 
 */
char* smsh_read_line(){

    long buffer_size = MAX_BUFFSIZE; // set our buffer size for getline.
    char* input_cmd = NULL; // we got an empty line.
    
    // Our parent shell ignores SIGINT
    //signal(SIGINT, smsh_sigint_handler); Save til later

    getline(&input_cmd, &buffer_size, stdin); 
    return input_cmd; // return the line of input they gave.
}

/**
 * @brief Given a parsed line of input, turn it into individual things based off delimiters.
 * 
 * @param input_line 
 * @return char** 
 */

char** smsh_split_line(char* input_line){
    // Parse the whole line based off of " ", \n
    char* token;
    char** tokens = malloc(MAX_BUFFSIZE * sizeof(char*)); // make a max size buffer of strings.
    int curr_word = 0; // Tracker.
    
    token = strtok(input_line, SM_DELIMS); // get from spaces and newlines.
    do{
        tokens[curr_word] = token; // place the token into the buffer we made.
        token = strtok(NULL, SM_DELIMS);
        curr_word++; // goto next word.
    }while(token != NULL); // Parse while token exists.

    tokens[curr_word] = NULL; // Terminate the token array.

    return tokens; 
}

int smsh_exec_cmd(char** args){

}

// ------------------------------------ START Signal Handlers


/**
 * @brief 
 * 
 */
void smsh_sigint_handler(){
    kill(fg_pid, SIGINT); // Kill the foreground process.
}


// ------------------------------------ END Signal Handlers