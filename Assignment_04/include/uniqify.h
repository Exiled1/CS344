#ifndef ASSIGNMENT_04_INCLUDE_UNIQIFY
#define ASSIGNMENT_04_INCLUDE_UNIQIFY

#include <stdio.h>

char** string_tokenize(char* input);
void sort_pipe_init(int task_num, fd_list_t input_fds, fd_list_t sorting_fds);
void string_sanitize(char* input);
void parser(int task_num, int** parse_buffers);

#define READLINE_STDIN(buffer, buffer_size) (fgets(buffer, buffer_size, stdin) != NULL)

#define PRINT_STR_LIST(word_list) while (*word_list) printf("%s\n", *word_list++)

#define TOKENIZE_INTO(word_list, src_buff) (word_list = string_tokenize(src_buff))


#endif /* ASSIGNMENT_04_INCLUDE_UNIQIFY */
