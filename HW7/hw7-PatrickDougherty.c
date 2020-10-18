/************************************************
 * CSci 451 - HW7
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 19 Oct 20
 * *********************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/************************************************
 * Global Variables
 * *********************************************/
pthread_mutex_t mutex;
const char* threadNames[] = {"Vlad", "Frank", "Bigfoot", "Casper", "Gomez"};
float balance = 0.0f;

/************************************************
 * Thread Function(s)
 * *********************************************/

void* threadUpdateBalance(void* arg)
{
    const char* threadName = (char*) arg;
    char fileName[12];
    char readVal[10];
    char transType;
    float transAmount;

    sprintf(fileName, "%s.in", threadName);
    FILE *inFile = fopen(fileName, "r");

    while(fscanf(inFile, "%c", &transType) != EOF)
    {
        if (transType == 'R')
        {
            fscanf(inFile, "\n");
            pthread_mutex_lock(&mutex);
        }
        else if (transType =='+')
        {
            fscanf(inFile, "%f\n", &transAmount);
            balance += transAmount;
        }
        else if (transType == '-')
        {
            fscanf(inFile, "%f\n", &transAmount);
            balance -= transAmount;
        }
        else if (transType == 'W')
        {
           printf("Account balance after thread %s is $%.2f\n", threadName, balance);
           pthread_mutex_unlock(&mutex);
           fscanf(inFile, "\n");
           usleep(1);           
        } 
        else
        {
            printf("Invalid transaction type encountered: %c\n", transType);
        }
    
            
    }
     
    fclose(inFile);

    pthread_exit(NULL);
}

/************************************************
 * Utility Functions
 * *********************************************/


/************************************************
 * Main
 * *********************************************/

int main (void)
{
    pthread_t thread[5];
    
    pthread_mutex_init(&mutex, NULL);
    
    for (int i = 0; i < 5; i++)
    {
        pthread_create(&thread[i], NULL, threadUpdateBalance, (void*) threadNames[i]);
    }
    
    for (int i = 0; i < 5; i++)
    {
        pthread_join(thread[i], NULL);
    }

    return 0;
}
