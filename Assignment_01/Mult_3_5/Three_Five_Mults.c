#include <stdio.h>
#include <stdlib.h>

// Easiest one of the lot LOL.

int main(int argc, char *argv[])
{
    //Get the argument, if we weren't provided one, error out and exit.
    if(argc != 2){
        fprintf(stderr, "Invalid amount of arguments supplied.\n");
        exit(1);
    }
    char *console_args = argv[1];
    int parsed_num = atoi(console_args);
    int total = 0;

    for (int i = 0; i < parsed_num; i++)
    {
        if (i % 3 == 0 || i % 5 == 0)
        {
            // Make our total go up for our multiples found.
            total += i;
        }
    }
    printf("Sum of the multiples of 3 or 5 for up to the given input: %d. %d\n", parsed_num, total);

    return 0;
}
