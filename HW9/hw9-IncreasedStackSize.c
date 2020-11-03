/************************************************
 * CSci 451 - HW9
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 2 Nov 20
 * *********************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#define NUM_OF_THREADS 2 

/************************************************
 * Global Variables
 * *********************************************/
pthread_mutex_t mutex;
struct threadData
{
    int threadNum;
    FILE *outFile;
    FILE *inFile;
    char fn[12];
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
    char readVal[30];
    FILE *outFile;

    while (fscanf(data->inFile, "%s\n", readVal) != EOF)
    {
       outFile = fopen(data->fn, "a");
       fprintf(outFile, "%s\t%d\n", readVal, data->threadNum);
       fclose(outFile);
       usleep(1);
    }
    
    pthread_exit(NULL);
}

void* threadMutex(void *arg)
{
    struct threadData *data = (struct threadData*)arg;
    char readVal[40];

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
    
    const rlim_t kStackSize =128 * 1024 * 1024;
    struct rlimit rl;
    int result;
    result = getrlimit(RLIMIT_STACK, &rl);
    printf("%d\n", result);

    if (result == 0)
    {
        if (rl.rlim_cur < kStackSize)
        {
            printf("%ld\n", rl.rlim_cur);
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0)
            {
                printf("setrlimit returned result = %d\n", result);
            }
        }
    }
    
    if (argc != 2)
    {
        printf("Invalid input. Usage: program_name <mutex | nomutex>\n");
        return 0;
    }
    pthread_attr_t attr;
    size_t size = 16 * 1024 * 1024;
    int ret = pthread_attr_setstacksize(&attr, size);
    printf("%d\n", ret);
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
            strcpy(data[i].fn, "hw9.nomutex");
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
            pthread_create(&thread[i], &attr, threadMutex, (void*) &data[i]);
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
