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

int sharedInt;      
pthread_mutex_t mutex;
int read = 1;       //used to indicate if value in sharedInt has been read
int written = 0;    //used to indicate if new value has been written to sharedInt
int done = 0;       //used to indicate end of file reached

/************************************************
 * Thread Functions
 * *********************************************/

void* fileReader(void* arg)
{
    int readVal;
    char temp;
    FILE *readFile = fopen("hw4.in", "r");
    
    while(1)
    {
        pthread_mutex_lock(&mutex);
        if(read)
        {
            read = 0;
            if(fscanf(readFile, "%d\n", &readVal) == EOF)
            {
                done = 1;
                written = 1;
                pthread_mutex_unlock(&mutex);
                break;
            }
            sharedInt = readVal;
            written = 1;
        }
        pthread_mutex_unlock(&mutex);
    }
    fclose(readFile);
    pthread_exit(NULL);
}

void* fileWriter(void* arg)
{
    FILE *fp = (FILE *) arg;
    int writeVal;

    while(1)
    {
        pthread_mutex_lock(&mutex);
        if(written)
        {
            written = 0;
            if(done)
            {
                read = 1;
                pthread_mutex_unlock(&mutex);
                break;
            }
            writeVal = sharedInt;
            read = 1; 
            pthread_mutex_unlock(&mutex); 
        
            if ((writeVal % 2) == 0)
            {
                fprintf(fp, "%d\n%d\n", writeVal, writeVal);
            }
            else
            {
                fprintf(fp, "%d\n", writeVal);
            }
            continue;
        }
        pthread_mutex_unlock(&mutex);

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


