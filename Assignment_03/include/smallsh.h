#ifndef ASSIGNMENT_03_INCLUDE_SMALLSH
#define ASSIGNMENT_03_INCLUDE_SMALLSH
#include "process.h"

void smsh_sigint_handler();
void shell_init();
char* smsh_read_line();
void check_buffer(int taken_size);
char** smsh_split_line(char* input_line);
int smsh_exec_cmd(char** args);
int smsh_exec_nonBIs(char** commands, bool background);
int smsh_exec_BIs(char** commands, bool* background);
char* smsh_expand_pid(char* pid_token);
int smsh_cd (char **args);
int smsh_status(int status);
int smsh_exit(char **args);
void smsh_push_process(Process_t** process_head, int pid);
void smsh_input_redir(char** commands, bool background);
void smsh_sigtstp_handler();
void check_bg_processes();

#endif /* ASSIGNMENT_03_INCLUDE_SMALLSH */
