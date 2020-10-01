/************************************************
 * CSci 451 - HW5
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 5 October 2020
 * *********************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/************************************************
 * Global Variable
 * *********************************************/
struct threadData
{
    char sharedChar;
    int turn;
    int count;
    int done;
    int thread;
};
pthread_cond_t full;
pthread_cond_t empty;
pthread_mutex_t mutex;
pthread_barrier_t barrier;

/************************************************
 * Thread Functions
 * *********************************************/

void* threadOne(void* arg)
{
    char readVal;
    int threadNum;
    struct threadData *data;
    //Access data for thread number while ensuring main doesn't change data
    printf("A thread started\n");
    pthread_mutex_lock(&mutex);
    data = (struct threadData *) arg;
    threadNum = data->thread;
    printf("Thread %d started\n", threadNum);
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&mutex);
    printf("Thread %d released mutex\n", threadNum);
    pthread_barrier_wait(&barrier);

    char fileName[9];
    sprintf(fileName, "hw5-%d.in", (threadNum + 1));
    FILE *readFile = fopen(fileName, "r");
    printf("Thread %d opened %s\n", threadNum, fileName);
    
    while (fscanf(readFile, "%c\n", &readVal) != EOF)
    {
        pthread_mutex_lock(&mutex);
        while (data->count > 0 || data->turn != threadNum)
        {
            printf("Thread %d waiting\n", threadNum);
            pthread_cond_wait(&full, &mutex);
        }
        printf("Thread %d done waiting\n", threadNum);
        data->sharedChar = readVal;
        data->count++;
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }
    if (threadNum == 0)
    //All files are the same size, therefore if one is done they all are
    {
        pthread_mutex_lock(&mutex);
        while (data->count > 0 || data->turn != threadNum)
        {
            printf("Thread 0 waiting to signal done\n");
            pthread_cond_wait(&full, &mutex);
        }
        printf("Thread 0 signaling done\n");
        data->done = 1;
        data->count++;
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }
    printf("Thread %d exiting\n", threadNum);

    fclose(readFile);
    pthread_exit(NULL);
}

/************************************************
 * Main
 * *********************************************/

int main(void)
{
    pthread_t thread[3];
    struct threadData data;
    data.turn = 0;
    data.count = 0;
    data.done = 0;
    char writeVal;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&full, NULL);
    pthread_barrier_init(&barrier, NULL, 3);
    
    FILE *writeFile = fopen("hw5.out", "w");

    for (int i = 0; i < 3; i++)
    {
        pthread_mutex_lock(&mutex);
        data.thread = i;
        pthread_create(&thread[i], NULL, threadOne, (void*) &data);
        pthread_cond_wait(&empty, &mutex); //wait until thread signals it read data
        pthread_mutex_unlock(&mutex);
        printf("Thread %d created\n", i);
    }
    printf("All threads created, starting main loop\n");

    while (1)
    {
        pthread_mutex_lock(&mutex);
        while (data.count < 1)
        {
            printf("Main waiting\n");
            pthread_cond_wait(&empty, &mutex);
        }
        printf("Main done waiting\n");
        if (data.done && data.turn == 0)
        {
            printf("Main signaled done\n");
            pthread_mutex_unlock(&mutex);
            break;
        }
        writeVal = data.sharedChar;
        data.count--;
        data.turn = (data.turn + 1) % 3;
        printf("Main recieved %c, turn now %d\n", writeVal, data.turn);
        pthread_cond_broadcast(&full);
        pthread_mutex_unlock(&mutex);
        fprintf(writeFile, "%c\n", writeVal);
    }
    printf("Main loop exited\n");
  
    for (int i=0; i<3; i++)
    {
        if(pthread_join(thread[i], NULL) != 0)
        {
            printf("Thread %d failed to join\n", i);
        }
        else
        {
            printf("Thread %d joined\n", i);
        }
    }
    printf("All threads joined, closing writeFile\n");

    fclose(writeFile);
    printf("File closed\n");

    if(pthread_barrier_destroy(&barrier) != 0)
        printf("barrier destroy failed\n");
    if(pthread_cond_destroy(&full) != 0)
        printf("full cond destroy failed\n");
    if(pthread_cond_destroy(&empty) != 0)
        printf("empty cond destroy failed\n");
    if(pthread_mutex_destroy(&mutex) != 0)
        printf("mutex destroy failed\n");
    
    printf("everything should be done\n");
    return 0;
    printf("This shouldn't happen\n");
}



