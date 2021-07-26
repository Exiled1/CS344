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
#include "smallsh.h"

#define SM_PROMPT_CHAR ": "
#define SM_DELIMS " \n"
#define COMMENT '#'
#define BASHPID "$$"
#define SM_BG_TOK "&"
#define MAX_BUFFSIZE 2048 // max buffer for any command.
#define ARGMAX 512  // max args for a command.
#define SUCCESS 0
#define FAILURE 1

int g_child_status = 0; // Child status is set to whatever the Process->exit_status is.
int g_fg_pid = 0; // Global foreground process PID tracker.
bool g_smsh_background_allowed = true; // For triggering non bg/bg mode

/**
 * @brief Shell driver code, doesn't take in arguments since that'd be weird.
 * 
 * @return int 
 */
int main(void)
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
        printf(SM_PROMPT_CHAR); // Print the : thingie, or something else.
        input_str = smsh_read_line(); // Read in input from stdin.
        args = smsh_split_line(input_str); // Split the line by ' ' and '\n'
        state = smsh_exec_cmd(args); // Execute valid commands.
        for(i = 0; args[i] != NULL; i++){
            printf("%s\n", args[i]);
        }
    }while(state != FAILURE);
}
// Driver code ^^ 

// ------------------------------------- END DRIVER CODE

// ------------------------------------- CONSTANTS AND FUNCTION LISTS START


// List of built in commands.

const char* smsh_builtin_commands[] = {
    "cd",
    "status",
    "exit"
};

// Function container, basically, using function pointers, you're able to call a list of functions and pass them arguments as well.
int (*smsh_bi_funcs[])(char** arguments) = {
    &smsh_cd,
    &smsh_status,
    &smsh_exit
};

/**
 * @brief Used to count the number of builtins that the shell has.
 * 
 * @return int 
 */
int smsh_builtin_num(){
    return sizeof(smsh_builtin_commands) / sizeof(char*);
}
// ------------------------------------- CONSTANTS AND FUNCTION LISTS END


// ----------------------------------- START PARSING

/**
 * @brief Read the lines that come in, no parsey parsey.
 * 
 * @return char* 
 */
char* smsh_read_line(){

    long buffer_size = MAX_BUFFSIZE; // set our buffer size for getline.
    char* input_cmd = NULL; // we got an empty line.
    // Our parent shell ignores SIGINT
    signal(SIGINT, smsh_sigint_handler); // Save til later
    
    getline(&input_cmd, &buffer_size, stdin);

    if(feof(stdin)){ // Check if we've recieved an EOF from the user after their getline.
        printf("Recieved an EOF\n");
        exit(EXIT_SUCCESS); // Got an EOF.
    }

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
// ------------------------------------ END OF PARSING

// ------------------------------------ COMMANDS START

/**
 * @brief Execute all commands. Return 1 if failed, 0 as success.
 * 
 * @param args 
 * @return int 
 */
int smsh_exec_cmd(char** commands){
    // TODO: Handle comments, $$, &
    // TODO: Handle builtin commands: exit, cd, status.
    int built_success = 0, nonbuilt_success = 0;
    bool background = false; // Setting background for later.
    
    built_success = smsh_exec_BIs(commands, &background); // Check  for built-ins, sets the & flag here.
    nonbuilt_success = smsh_exec_nonBIs(commands, background); // Check for non built ins.
    // int i = 0;

    if(built_success == FAILURE || nonbuilt_success == FAILURE){
        return FAILURE;
    }
    return SUCCESS;
}


/**
 * @brief Takes care of comments, $$, and & built-ins, these commands aren't allowed to be run in the background.
 * 
 * @param commands 
 * @return int 
 */

int smsh_exec_BIs(char** commands, bool* background){
    int current_arg = 0;
    // TODO: # Done.
    if(commands[0] == NULL || strchr(*commands, COMMENT) != NULL){
        printf("Comment/NULL recieved.\n");
        return SUCCESS;
    }
    // TODO: $$ and &
    while(commands[current_arg] != NULL){ // Go through the commands list.
        if(strstr(commands[current_arg], BASHPID)){ // if we find a $$, get the PID as a string.
            // We don't need strstrs string pointer since we're at the argument anyways.
            commands[current_arg] = smsh_expand_pid(commands[current_arg]); // Replace the current argument with the PID.
        }
        current_arg++; // Poggers.
    }

    if(current_arg - 1 != 0 && strcmp(commands[--current_arg], SM_BG_TOK) == SUCCESS){ // Our last argument has the ability to be the background token.
        commands[current_arg] = NULL; // Make this argument disappear, set background flag.
        printf("& Flag recieved.\n");
        *background = true; // Run the rest of the things in background mode.
    }

    // TODO: Run through the builtins, done.
    // ? Loop through the list of commands, then execute that command if it matches the given command.

    for(current_arg = 0; current_arg < smsh_builtin_num(); current_arg++){
        if(strcmp(commands[0], smsh_builtin_commands[current_arg]) == SUCCESS){ // If we've matched the argument to an existing builtin, execute that builtin.
            return (*smsh_bi_funcs[current_arg])(commands); // ? execute the command with the given arguments.
        }
    }

    return SUCCESS;
}

/**
 * @brief Execute non built in commands. These can be run in the background.
 * 
 * @param commands 
 * @return int 
 */
int smsh_exec_nonBIs(char** commands, bool background){
    /**
     * @brief Make processes, record their IDs with their input/output files, statuses.
     * handle '<', '>', also background process handling to /dev/null,
     * 
     */
    return SUCCESS;
}

// --------------------------- PROCESS CHECKING.
/**
 * @brief Checks for terminated processes.
 * 
 * @param process_list 
 */
void smsh_check_processes(Process_t* process_list){
    printf("check_processes WIP.\n");
}


// --------------------------- BASHPID stuff.

/**
 * @brief Whenever this is called, replace the old output with this <3
 * 
 * @param pid_token 
 * @return char* 
 */
char* smsh_expand_pid(char* pid_token){
    //TODO: Currently replaces the entire word if it's in a word, fix this later.
    pid_t shell_pid = getpid(); // Get my shell pid. -> 12
    // 12 -> "12"
    int buf_len = snprintf(NULL, 0, "%d", shell_pid); // Start a dry run to get the size of the pid - \0
    char* pid_buf = malloc(sizeof(char) * buf_len + 1); // allocate a buffer with the length of the pid PLUS the \0
    snprintf(pid_buf, buf_len + 1, "%d", shell_pid); // int -> str + \0
    printf("Returning %s\n", pid_buf);
    return pid_buf;
}

// ------------------------------------ Built in command functions
// exit, status, cd
/**
 * @brief Changes the directory of smallsh, if given no arguments it'll change directories based off of your home path, if given an argument it'll traverse 
 * 
 * @param args 
 * @return int 
 */
int smsh_cd (char **arguments){ // ? complete 
    long dir_size;
    char* buff;
    char* ptr;

    // ? with no arguments, go to home.
    if(arguments[1] == NULL){
        chdir(getenv("HOME"));
    } else{    // With arguments, change directories to that path.
        if(chdir(arguments[1]) == -1){
            fprintf(stderr, "Failed to change directories, please make sure the directory exists or is a directory.");
            // Shouldn't exit, since they could have gone to a non existent directory, or a file isn't a directory.
            fflush(stdout); // Flush the buffers to stdout so my error works.
        }
    }
    // ! Remove this debug code later. 
    dir_size = pathconf(".", _PC_PATH_MAX);
    if ((buff = (char *)malloc((size_t)dir_size)) != NULL) // testing code from man pages.
        ptr = getcwd(buff, (size_t)dir_size);

    printf("%s\n", buff);
    return SUCCESS;
}

/**
 * @brief Print the exit status of a process.
 * 
 * @param args 
 * @return int 
 */
int smsh_status(char **args){ // maybe done.
    //TODO: Print the exit status of a process.
    if(WIFEXITED(g_child_status)){
        printf("Normal exit. Status: %d\n", WEXITSTATUS(g_child_status));
    }else{
        printf("Abnormal termination. Status: %d\n", WTERMSIG(g_child_status));
    }

    return SUCCESS;
}
/**
 * @brief Exit process, called when exit is typed, removes the group processes.
 * 
 * @param args 
 * @return int 
 */
int smsh_exit(char **args){
    kill(g_fg_pid, SIGTERM); // Kindly ask the group processes to terminate.
    return FAILURE; // Not actually a failure, but I have this set to loop til it fails.
}


// ------------------------------------ START Signal Handlers


/**
 * @brief 
 * 
 */
void smsh_sigint_handler(){
    //kill(g_fg_pid, SIGINT); // Kill the foreground process.
    printf("Recieved a SIGINT. WIP.\n");
    printf(": ");
    fflush(stdout);
}


// ------------------------------------ END Signal Handlers