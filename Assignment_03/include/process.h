#ifndef ASSIGNMENT_03_INCLUDE_PROCESS
#define ASSIGNMENT_03_INCLUDE_PROCESS
#include <sys/wait.h>

typedef struct process Process_t;

typedef struct process{
    pid_t pid;
    int exit_status; // initialize this with 0.
    Process_t* next; // link the bg processes.
}Process_t;

#endif /* ASSIGNMENT_03_INCLUDE_PROCESS */
