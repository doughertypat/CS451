/************************************************
 * CSci 451 - HW9
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 2 Nov 20
 * *********************************************/
/*
 * Analysis - Output is jumbled in the nomutex output while the mutex option keeps the order
 * the same as the input file (or at least very close).  Both options seem to have a random
 * thread printing the values with no decernable pattern.  The mutex option takes much
 * longer to complete than the nomutex option. 
 */


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#define NUM_OF_THREADS 10

/************************************************
 * Global Variables
 * *********************************************/
pthread_mutex_t mutex;
struct threadData
{
    int threadNum;
    FILE *outFile;
    FILE *inFile;
};

/************************************************
 * Function Declarations
 * *********************************************/


/************************************************
 * Thread Functions
 * *********************************************/
 void* threadNoMutex(void* arg)
{
    struct threadData *data = (struct threadData*)arg;
    char readVal[100];
    
    while (fscanf(data->inFile, "%s\n", readVal) != EOF)
    {
       fprintf(data->outFile, "%s\t%d\n", readVal, data->threadNum);
       //usleep(1);
    }
    
    pthread_exit(NULL);
}

void* threadMutex(void *arg)
{
    struct threadData *data = (struct threadData*)arg;
    char readVal[100];

    while (1)
    {
        pthread_mutex_lock(&mutex);
        if (fscanf(data->inFile, "%s\n", readVal) == EOF)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        fprintf(data->outFile, "%s\t%d\n", readVal, data->threadNum);

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}


/************************************************
 * Helper Functions
 * *********************************************/

/************************************************
 * Main (Driver Function)
 * *********************************************/
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Invalid input. Usage: program_name <mutex | nomutex>\n");
        return 0;
    }
    pthread_t thread[NUM_OF_THREADS];
    struct threadData data[NUM_OF_THREADS];
    FILE *outFile;
    //TODO: create output file using "w" before creating threads
    FILE *inFile = fopen("hw9.data", "r");
    if (!strcmp(argv[1], "nomutex"))
    {
        outFile = fopen("hw9.nomutex", "w");
        for (int i = 0; i < NUM_OF_THREADS; i++)
        {
            data[i].threadNum = i;
            data[i].outFile = outFile;
            data[i].inFile = inFile;
            pthread_create(&thread[i], NULL, threadNoMutex, (void*) &data[i]);
        }
    }
    else if (!strcmp(argv[1], "mutex"))
    {
        pthread_mutex_init(&mutex, NULL);
        outFile = fopen("hw9.mutex", "w");
        for (int i = 0; i < NUM_OF_THREADS; i++)
        {
            data[i].threadNum = i;
            data[i].outFile = outFile;
            data[i].inFile = inFile;
            pthread_create(&thread[i], NULL, threadMutex, (void*) &data[i]);
        }   
    }
    else
    {
        printf("Invalid argument. Usage: program_name <mutex | nomutex>\n");
        return 0;
    }
    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        pthread_join(thread[i], NULL);
    }

    fclose(inFile);
    fclose(outFile);

    return 0;
}


