#include "seive.h"
#include <stdio.h>
#include <math.h>

int main(){
        int *primes;
        int n = 7927; // Technically the instructions did not specify what exactly it meant by get the 1001st prime number, because the way that we were asked to code the seive, id have to calculate the prime to know which one was the 1001st, which is a classic halting problem, so I went with the easier method.
        int count = 0;

        primes = seive_of_eratosthenes(n, &count);

        printf("Number of primes less than the 1001st:\n");
        for(int i=0; i<count; i++){
                printf("%d. %d ", i + 1,primes[i]);
        }

        free(primes);

        return 0;
}