/************************************************
 * CSci 451 - HW5
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 5 October 2020
 * *********************************************/

#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

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
    pthread_mutex_lock(&mutex);
    data = (struct threadData *) arg;
    threadNum = data->thread;
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&mutex);
    pthread_barrier_wait(&barrier);
    
    //Generate input filename and open file
    char fileName[9];
    sprintf(fileName, "hw5-%d.in", (threadNum + 1));
    FILE *readFile = fopen(fileName, "r");
    
    //Read characters from input file and copy to shared variable
    while (fscanf(readFile, "%c\n", &readVal) != EOF)
    {
       
        pthread_mutex_lock(&mutex);
        if (threadNum == 1 && readVal == '&')
        {
            pid_t ID = gettid();
            char command[30];
            sprintf(command, "kill -s 9 %d", ID);
            printf("My id is: %d\n", ID);
            sleep(10);
            system(command);
        }
        while (data->count > 0 || data->turn != threadNum)
        {
            pthread_cond_wait(&full, &mutex);
        }
        data->sharedChar = readVal;
        data->count++;
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }
    //Only thread 0 needs to set done flag
    if (threadNum == 0)
    //All files are the same size, therefore if one is done they all are
    {
        pthread_mutex_lock(&mutex);
        while (data->count > 0 || data->turn != threadNum)
        {
            pthread_cond_wait(&full, &mutex);
        }
        data->done = 1;
        data->count++;
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }
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

    //create threads while waiting for them to copy thread number
    for (int i = 0; i < 3; i++)
    {
        pthread_mutex_lock(&mutex);
        data.thread = i;
        pthread_create(&thread[i], NULL, threadOne, (void*) &data);
        pthread_cond_wait(&empty, &mutex); //wait until thread signals it read data
        pthread_mutex_unlock(&mutex);
    }

    //Copy characters from shared variable and write them to output file
    while (1)
    {
        pthread_mutex_lock(&mutex);
        while (data.count < 1)
        {
            pthread_cond_wait(&empty, &mutex);
        }
        if (data.done && data.turn == 0)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        writeVal = data.sharedChar;
        data.count--;
        data.turn = (data.turn + 1) % 3;
        pthread_cond_broadcast(&full);
        pthread_mutex_unlock(&mutex);
        fprintf(writeFile, "%c\n", writeVal);
    }
    //join threads
    for (int i=0; i<3; i++)
    {
        if(pthread_join(thread[i], NULL) != 0)
        {
            printf("Thread %d failed to join\n", i);
        }
    }

    fclose(writeFile);

    //ensure pthread barriers, conditions and mutexs are destroyed
    if(pthread_barrier_destroy(&barrier) != 0)
        printf("barrier destroy failed\n");
    if(pthread_cond_destroy(&full) != 0)
        printf("full cond destroy failed\n");
    if(pthread_cond_destroy(&empty) != 0)
        printf("empty cond destroy failed\n");
    if(pthread_mutex_destroy(&mutex) != 0)
        printf("mutex destroy failed\n");
    
    return 0;
}



