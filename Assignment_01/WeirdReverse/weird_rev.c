#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
 
long get_file_size(FILE *);
// I found most of the given things over complicated instead of just using file pointers, file descriptors seem more fitting when using things at the kernel level like sockets, pages, and stuff like that, using them here is kind of overkill in my opinion.
int main(int argc, char** argv)
{
    char *input = argv[1];
    char *output = argv[2];
    long total_bytes;
    char curr_byte;
    FILE *input_fp, *output_fp;
 
    if ((input_fp = fopen(input, "r")))
    {
        printf("The FILE has been opened...\n");
        output_fp = fopen(output, "w");
        total_bytes = get_file_size(input_fp); // to total_bytes the total number of characters inside the source file
        fseek(input_fp, -1, SEEK_END);     // makes the pointer input_fp to point at the last character of the file
        printf("Number of characters to be copied %lu\n", total_bytes);
 
        while (total_bytes > 0)
        {
            curr_byte = fgetc(input_fp); // Read the next character from our input file stream.
            fputc(curr_byte, output_fp); // Write our current byte to the output file.
            fseek(input_fp, -2, SEEK_CUR); // shifts the pointer to the previous character.
            total_bytes--;
        }
        printf("\nCompleted in reversing the bytes of the file! :D Thank you.\n");
    }
    else
    {
        fprintf(stderr, "An error occured while attempting to open the designated input file.\n");
        exit(1);
    }
    // close out both streams
    fclose(input_fp); 
    fclose(output_fp);
}

// Get the size of the file in a cleaner method. Well, it removes redundant code and helps me isolate my seeks.
long get_file_size(FILE *file_ptr) 
{
    fseek(file_ptr, -1, SEEK_END); // Go to the end of the file pointer
    long total_bytes = ftell(file_ptr); // use ftell to get the current size of the file.
    total_bytes++; // Add 1 so that we keep consistent with the bytes, since ftell returns everything minus 1 byte for some reason :shrug:
    return total_bytes;
}