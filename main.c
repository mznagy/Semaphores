#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SIZE 5 //used for buffer size
#define THREADSNUM 10 //number of counter threads
int counter = 0;//counter used for mCounter
sem_t counter_sem, buffer_sem, full_sem, empty_sem;//Semaphores
//counter_sem for mCounter to ensure that no one modifies counter
//buffer_sem used to make no two threads produce and consume at same time
//full_sem used to keep track of buffer and to block monitor if full
//empty_sem used to keep track of buffer and block collector if empty

int queue[SIZE], front = 0, rear = 0, itemCount = 0;//variables used in storing in queue

void enqueue(int value) {
    if (itemCount != SIZE) {//if there is empty place in buffer insert
        printf("Monitor thread: writing to buffer at position %d\n\n", rear);
        queue[rear++] = value;//adding new value in queue
        rear = rear % SIZE;//if rear reached end of queue reset it (circular queue)
        itemCount++;//increasing count
    } else {
        printf("Monitor thread: Buffer Full! \n\n");
    }
}

void dequeue() {
    if (itemCount == 0) {
        printf("Collector thread: Buffer empty \n\n");
    } else {
        printf("Collector thread: reading from buffer at position %d and value =%d\n\n", front, queue[front]);
        front++;//increasing front to next item
        front = front % SIZE;//checking if it reached the of and queue and reset it does
        itemCount--;//decreasing count
        if (itemCount == 0) {//if itemCount ==0 then reset both indexes
            front = 0;
            rear = 0;
        }
    }
}

int generateRandom(int a, int b) {
    return rand() % a + b;//generating a random number to be used in sleep
}

_Noreturn void *addCounter(void *arg) {
    int threadNum = *((int *) arg);
    while (1) {
        sleep(generateRandom(2, 1));
        printf("Counter thread %d: received a message\n\n", threadNum);//sleep done
        printf("Counter thread %d: Waiting to write\n\n", threadNum);//Waiting before sem_wait
        sem_wait(
                &counter_sem);//ensuring that no two threads from mCounter or mMonitor are modifying in the counter at the same time
        /*Critical Section start*/
        counter++;//Critical Section increasing counter
        /*Critical Section end*/
        printf("Counter thread %d: now adding to counter,counter value=%d\n\n", threadNum,
               counter);//printing new counter value
        sem_post(&counter_sem);//finishing critical section and enabling another thread to enter
    }
}


_Noreturn void *monitor(void *arg) {
    while (1) {
        sleep(generateRandom(3, 1));//generating random number
        printf("Monitor thread: waiting to read counter\n\n");//after sleep activation

        sem_wait(&counter_sem);//used to ensure that no counter thread is modifying the counter while resetting it here
        /*Critical Section start*/
        int tempVariable = counter;//putting the counter in a tempVariable to be added in the queue
        printf("Monitor thread: reading a count value of %d\n\n", counter);
        counter = 0;//resetting counter
        /*Critical Section end*/
        sem_post(&counter_sem);//finishing critical section and enabling the counter to continue

        int temp;//a temp used to check the value of semaphore to know if buffer is full
        sem_getvalue(&full_sem, &temp);//getting value of semaphore
        if (temp ==
            SIZE) {//checking if the value is full==SIZE then this means that full was incremented till it reached the size and buffer is full
            printf("Monitor thread: Buffer full! \n\n");
        }
        sem_wait(&empty_sem);//check if buffer is full and if it is then wait
        sem_wait(&buffer_sem);//ensuring that monitor and collector aren't in critical section at the same time
        /*Critical Section start*/
        enqueue(tempVariable);//Enqueuing in buffer and printing is done inside the function
        /*Critical Section end*/
        sem_post(&buffer_sem);//increasing semaphore to enable the collector to continue
        sem_post(&full_sem);//increasing semaphore full because we added an item in the buffer
    }
}

_Noreturn void *collect(void *arg) {
    while (1) {
        sleep(generateRandom(3, 1));
        int temp;
        sem_getvalue(&empty_sem, &temp);//getting value of empty_sem
        if (temp ==
            SIZE) {//if empty == SIZE it means that we didn't put anything in the buffer bec empty_sem is initialized with SIZE
            printf("Collector thread: nothing is in the buffer! \n\n");
        }
        sem_wait(&full_sem);//checking if buffer is empty if it is then wait
        sem_wait(&buffer_sem);//ensuring that monitor and collector aren't in critical section at the same time
        /*Critical Section start*/
        dequeue(counter);//removing from buffer and the print is done inside the function
        /*Critical Section end*/
        sem_post(&buffer_sem);//increasing semaphore to enable the monitor to continue
        sem_post(&empty_sem);//increasing semaphore empty because we took an item from the buffer
    }
}


int main() {
    //Initializing semaphores
    sem_init(&counter_sem, 0,
             1);//initializing counter_sem with 1 which means that only one thread can access critical section at a time
    sem_init(&empty_sem, 0,
             SIZE);//initializing empty_sem with SIZE which means that we can insert up to SIZE items in the buffer
    sem_init(&full_sem, 0,
             0);//Initialzing full_sem with zerthe buffer: Monitor threado which means that we can take up to SIZE items from the buffer
    sem_init(&buffer_sem, 0, 1);//Used for making one thread at a time in the critical section

    srand(time(0));//used to make rand() function generate different numbers
    int n = THREADSNUM;
    pthread_t mMonitor, mCollector, mCounter[n];

    int threadData[n];//array used to pass the thread number.
    for (int i = 0; i < n; i++) {
        threadData[i] = i + 1;
        pthread_create(&mCounter[i], NULL, addCounter, &threadData[i]);//creating N mCounter threads
    }
    pthread_create(&mMonitor, NULL, monitor, NULL);//creating one thread for mMonitor
    pthread_create(&mCollector, NULL, collect, NULL);//creating one thread for mCollector

    for (int i = 0; i < n; ++i) {
        pthread_join(mCounter[i], NULL); //joining threads of N mCounter
    }
    pthread_join(mMonitor, NULL); //joining mMonitor thread
    pthread_join(mCollector, NULL); //joining mCollector thread
    return 0;
}