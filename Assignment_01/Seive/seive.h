#ifndef ASSIGNMENT_01_SEIVE
#define ASSIGNMENT_01_SEIVE

#include <stdlib.h>
#include <math.h>

//returns base address of primes[] array containing prime numbers less than given limit
//also, sets count variable passed as reference to number of primes less than given limit

int *seive_of_eratosthenes(int limit, int *count)
{
    int index, curr_mult, curr_prime, marker;
    int *primes; // To store the prime numbers in.
    int *markers = (int *)calloc(limit, sizeof(int)); // Initialize everything from 0 to n as 0, its a surprise tool that'll help us later.

    //initializing with 0 to show non special numbers.
    for (index = 0; index < limit; index++)
    {
        markers[index] = 0;
    }

    //index is 1 less than number
    markers[0] = -1; //So, markers[0] = -1, marking number 1 as special

    *count = limit - 1; // 
    //resetting curr_prime = 1, im lazy.
    curr_prime = 1;

    while (curr_prime < sqrt(limit))
    {

        //finding first number greater than curr_prime which isn't composite. Poggers.
        for (index = curr_prime + 1; index <= limit; index++)
        {
            if (markers[index - 1] == 0)
            {
                marker = index;
                break;
            }
        }

        for (curr_mult = marker * marker; curr_mult <= limit; curr_mult += marker)
        {
            if (markers[curr_mult - 1] == 0)
            {
                markers[curr_mult - 1] = 1; //marking all multiples of marker as 1
                (*count)--;
            }
        }

        //setting curr_prime = marker
        curr_prime = marker;
    }

    primes = (int *)calloc((*count), sizeof(int));

    curr_mult = 0;

    //adding all unmarked numbers to primes[] array
    for (index = 0; index < limit; index++)
    {
        if (markers[index] == 0)
        {
            primes[curr_mult] = (index + 1);
            curr_mult++;
        }
    }

    free(markers);

    return primes;
}

#endif /* ASSIGNMENT_01_SEIVE */
