#ifndef ASSIGNMENT_03_INCLUDE_SMALLSH
#define ASSIGNMENT_03_INCLUDE_SMALLSH

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
int smsh_status(char **args);
int smsh_exit(char **args);

#endif /* ASSIGNMENT_03_INCLUDE_SMALLSH */