# Producer-Consumer Problem in C
## Problem statement
You are required to write a C program to solve the following synchronization problem using
POSIX and “semaphore.h” libraries.
N mCounter threads count independent incoming messages in a system and another thread
mMonitor gets the count of threads at time intervals of size t1, and then resets the counter to
0. The mMonitor then places this value in a buffer of size b, and a mCollector thread reads
the values from the buffer.
Any thread will have to wait if the counter is being locked by any other thread. Also, the
mMonitor and mCollector threads will not be able to access the buffer at the same time or to
add another entry if the buffer is full.
Assume that the messages come randomly to the system, this can be realized if the mCounter
threads sleep for random times, and their activation (sleep time ends) corresponds to an email
arrival. Similarly, the mMonitor and mCollector will be activated at random time intervals.
## Files
main.c: Contains the main logic of the program.
## Description
The program creates a fixed size buffer and a number of producer and consumer threads. The producers generate a sequence of integers and place them into the buffer. The consumers remove these integers from the buffer and print them.

The buffer is implemented as a circular queue. The program uses semaphores to ensure that the producers and consumers access the buffer in a mutually exclusive manner. It also uses semaphores to keep track of the number of items in the buffer, blocking the producers if the buffer is full and the consumers if the buffer is empty.

## Variables
SIZE: The size of the buffer.
THREADSNUM: The number of producer and consumer threads.
counter: A counter used for mCounter.
counter_sem, buffer_sem, full_sem, empty_sem: Semaphores used to control access to the buffer and the counter.
queue, front, rear, itemCount: Variables used to implement the buffer as a circular queue.
## Functions
enqueue(int value): Inserts a value into the buffer if there is space available.
## How to Run
To compile the program, use the following command:
gcc -o main main.c -lpthread -lrt
To run the program, use the following command:
./main
Please note that this program uses POSIX semaphores, which may not be available on all platforms.
