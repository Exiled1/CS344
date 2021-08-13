# Assignment 4: Multi Threaded Merge Sort

Compilation for threads:
Navigate to the makefile, then go to the line that contains FT_MACROS, and replace the THREAD/PROCESS word, with THREAD
then type make, then run `./uniqify n_processes < input.txt > output.txt`

In between compilation steps, you must `make clean`, otherwise it will not swap the compilation modes.

Compilation for Processes:
Navigate to the makefile, then go to the line that contains FT_MACROS, and replace the THREAD/PROCESS word, with PROCESS
then type make, then run `./uniqify n_processes < input.txt > output.txt`

Question And Answer:

1. Are there benefits to using threads vs processes and vice versa???
    - Yes, there are definitely benefits to using threads and processes and the other way around. First, threads are
    incredibly cheap in terms of resources. Spawning them is not nearly as cost heavy as spawning a new process.
    Threads allow the programmer to use the same memory space and syncronize thread access to that memory space. 
    This is incredibly useful when you need to have a bunch of small tasks. Threads can do anything a process can do
    however, processes can spawn a bunch of threads. Threading also carries the advantage of being able to easily communicate
    between threads, since all of a process share the same address space. Because of this ability to share a memory space,
    you can have many tasks running in parallel.
    - Processes are more fit as heavy duty agents to take care of high importance tasks that you want to dedicate to 
    a CPU such as dealing with 

2. Which variation runs with higher performance? Did this surprise you?
    - Technically, for small values of K, processes should be faster, however with a larger value of K over a larger
     amount of data, threads should win out in terms of processing speed. Since the time to spawn a thread is 
     negligable since the sort process should take longer to complete than the thread takes to spawn.
    - Did it surprise me? Not really, it's a 

Are there benefits to using threads vs processes:
Yes, there are definitely benefits to using threads over processes, since sharing resources as inner process 
communication would be needlessly complicated and resource heavy. Threads have the added benefit of sharing
the same memory space. 

    ASCII Data tables with N = 5 tests

    Small File: 12 KiB
    +----+-----------------+------------------+
    | K  | Thread(seconds) | Process(seconds) |
    +----+-----------------+------------------+
    |  1 | 0:00.02         | 0:00.02          |
    |  2 | 0:00.01         | 0:00.02          |
    |  3 | 0:00.01         | 0:00.02          |
    |  4 | 0:00.01         | 0:00.01          |
    |  5 | 0:00.01         | 0:00.01          |
    |  6 | 0:00.02         | 0:00.02          |
    |  7 | 0:00.01         | 0:00.02          |
    |  8 | 0:00.01         | 0:00.02          |
    |  9 | 0:00.01         | 0:00.01          |
    | 10 | 0:00.02         | 0:00.01          |
    | 15 | 0:00.02         | 0:00.02          |
    | 20 | 0:00.02         | 0:00.02          |
    | 25 | 0:00.01         | 0:00.01          |
    | 30 | 0:00.01         | 0:00.02          |
    +----+-----------------+------------------+

    Medium File: 145 KiB
    +----+-----------------+------------------+
    | K  | Thread(seconds) | Process(seconds) |
    +----+-----------------+------------------+
    |  1 | 0:00.13         | 0:00.10          |
    |  2 | 0:00.11         | 0:00.10          |
    |  3 | 0:00.10         | 0:00.07          |
    |  4 | 0:00.08         | 0:00.09          |
    |  5 | 0:00.08         | 0:00.07          |
    |  6 | 0:00.07         | 0:00.07          |
    |  7 | 0:00.07         | 0:00.07          |
    |  8 | 0:00.08         | 0:00.07          |
    |  9 | 0:00.06         | 0:00.07          |
    | 10 | 0:00.07         | 0:00.07          |
    | 15 | 0:00.05         | 0:00.07          |
    | 20 | 0:00.06         | 0:00.06          |
    | 25 | 0:00.08         | 0:00.07          |
    | 30 | 0:00.08         | 0:00.06          |
    +----+-----------------+------------------+

    Big File: 2 MiB
    +----+-----------------+------------------+
    | K  | Thread(seconds) | Process(seconds) |
    +----+-----------------+------------------+
    |  1 | 0:03.52         | 0:02.34          |
    |  2 | 0:02.93         | 0:02.48          |
    |  3 | 0:02.85         | 0:02.41          |
    |  4 | 0:02.24         | 0:01.90          |
    |  5 | 0:01.55         | 0:01.91          |
    |  6 | 0:02.44         | 0:01.96          |
    |  7 | 0:01.53         | 0:02.98          |
    |  8 | 0:01.63         | 0:02.79          |
    |  9 | 0:02.04         | 0:02.67          |
    | 10 | 0:02.07         | 0:02.41          |
    | 15 | 0:02.13         | 0:01.89          |
    | 20 | 0:03.05         | 0:02.14          |
    | 25 | 0:02.30         | 0:02.88          |
    | 30 | 0:01.55         | 0:02.36          |
    +----+-----------------+------------------+

