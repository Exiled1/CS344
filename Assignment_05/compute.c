#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>

static inline uint64_t rdtsc() {
        uint32_t lo, hi;
        __asm__ __volatile__ ("xorl %%eax, %%eax\n cpuid\n rdtsc\n"
                              : "=a" (lo), "=d" (hi)
                              :
                              : "%ebx", "%ecx");
        return (uint64_t)hi << 32 | lo;
}

#define ITERS 10000

void compute(){

}

int main(int argc, char **argv)
{
        //get CPU clock speed, assumes the following is running in the background or in another logged in terminal:
        // (while true ; do cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_cur_freq | R -q -e "x <- read.csv(file('stdin'), header = F); summary(x); sd(x[ , 1])" | grep Mean | cut -f2 -d':' >| ~/ticks.txt; sleep 1; done) &
        //make sure to kill that loop when you're done with testing/running your code -- bring it to the foreground with fg then hit control-C
        //there may be better ways to do this, but that gives the current speed, rather than max speed
        FILE *f = fopen("ticks.txt", "r");
        long ticks;
        fscanf(f, "%ld", &ticks);

        //start timing!
        uint64_t time = rdtsc(), total;
        for(volatile int i = 0; i < ITERS; ++i){
                // I think this is where I place compute.
                compute(); // Compute perfect numbers.
        }
        //get time for timed operation AND looping
        total = rdtsc() - time;

        time = rdtsc();
        for(volatile int i = 0; i < ITERS; ++i){
                //don't actually do anything here, this measures loop overhead
        }
        //subtract off loop overhead!
        total -= (rdtsc() - time);



        //you won't actually be printing, but this shows your results
        //if numbers seem weird, increase iterations or number of copied lines of code of timed operation
        printf("%d iterations took %lu cycles\n", ITERS, total);
        printf("average iteration time is %f seconds (clockspeed = %ld Hz)\n", (double)total/(double)ticks, ticks);

        return 0;
}