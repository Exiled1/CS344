#ifndef ASSIGNMENT_04_INCLUDE_MSORT
#define ASSIGNMENT_04_INCLUDE_MSORT

#include <stdio.h>
#include <pthread.h>
typedef int** Pipe_list_t;
typedef int* pid_list_t;

typedef struct uniq_proc{
    Pipe_list_t parse_pipes;
    Pipe_list_t sort_pipes;
    pid_list_t pid_list;
    int task_num;
    pthread_cond_t conditions[2];
}Uniq_proc_t;

typedef struct word_track{
    char word[400];
    int occurrences;
}word_track_t;
void fork_silencer(Uniq_proc_t* process);
void *thread_sort_caller(Uniq_proc_t *process);
void *thread_parse_caller(Uniq_proc_t *process);
void *thread_silence_caller(Uniq_proc_t *process);
void close_pipes_array(int **pipe_array, int end);
char** string_tokenize(char* input);
void sort_pipe_init(Uniq_proc_t* process);
void string_sanitize(char* input);
void parser(Uniq_proc_t* process);
void silencer(Uniq_proc_t* process);
int lexic_min_at(int buffer_num, char** buffer_heaps);

//void pipe_send_data(Uniq_proc_t* process, char** word_list, FILE** stream_buffers, int counter);
#define READLINE_STDIN(buffer, buffer_size) (fgets(buffer, buffer_size, stdin) != NULL)

#define PRINT_STR_LIST(word_list) while (*word_list) printf("%s\n", *word_list++)

#define TOKENIZE_INTO(word_list, src_buff) (word_list = string_tokenize(src_buff))


#endif /* ASSIGNMENT_04_INCLUDE_MSORT */
