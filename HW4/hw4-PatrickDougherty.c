/************************************************
 * CSci 451 - HW4
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 28 Sep 20
 * *********************************************/

#include <pthread.h>
#include <stdio.h> //printf, fprintf, FILE

/************************************************
 * Global Variables
 * *********************************************/

int sharedInt;      //essentially a buffer of size one
pthread_mutex_t mutex;
pthread_cond_t full;
pthread_cond_t empty;
int count = 0;      //count of items in buffer
int done = 0;       //used to indicate end of file reached

/************************************************
 * Thread Functions
 * *********************************************/

void* fileReader(void* arg)
{
    int readVal;
    char temp;
    FILE *readFile = fopen("hw4.in", "r");
    
    while (fscanf(readFile, "%d\n", &readVal) != EOF)
    {
        pthread_mutex_lock(&mutex);
        while (count > 0)
        {
            pthread_cond_wait(&full, &mutex);
        }
        sharedInt = readVal;
        count++;
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }
    //EOF reached...time to signal done
    pthread_mutex_lock(&mutex);
    while (count > 0) //ensure the last int was read
    {
        pthread_cond_wait(&full, &mutex);
    }
    done = 1;
    count++;
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&mutex);
    fclose(readFile);
    pthread_exit(NULL);
}

void* fileWriter(void* arg)
{
    FILE *fp = (FILE *) arg;
    int writeVal;

    while (1)
    {
        pthread_mutex_lock(&mutex);
        while (count < 1)
        {
            pthread_cond_wait(&empty, &mutex);
        }
        if (done)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        writeVal = sharedInt;
        count--;
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);
        if ((writeVal % 2) == 0)
            {
                fprintf(fp, "%d\n%d\n", writeVal, writeVal);
            }
        else
            {
                fprintf(fp, "%d\n", writeVal);
            }
    }
    pthread_exit(NULL);
}
    
/************************************************
 * Main
 * *********************************************/

int main(void)
{
    pthread_t thread[2];
    
    if (pthread_mutex_init(&mutex, NULL) != 0) 
    {
        printf("Mutex init failed");
        return 1;
    }
    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&full, NULL);

    FILE *writeFile = fopen("hw4.out", "w");

    pthread_create(&thread[0], NULL, fileReader, NULL);
    pthread_create(&thread[1], NULL, fileWriter, (void *) writeFile);

    for (int i=0; i<2; i++)
    {
        pthread_join(thread[i], NULL);
    }
    
    fclose(writeFile);

    return 0;
}


