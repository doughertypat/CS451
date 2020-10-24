/************************************************
 * CSci 451 - HW8
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 26 Oct 20
 * *********************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>  //rand_r
#include <unistd.h>

#define NUM_OF_THREADS 5

/************************************************
 * Global Variables
 * *********************************************/
pthread_mutex_t mutex;
const char* threadNames[] = {"Vlad", "Frank", "Bigfoot", "Casper", "Gomez"};
float balance = 0.0f;
int finished = 0;
struct Page
{
    int R;
    int M;
    int owner;
    struct Page *next;
    pthread_mutex_t pageMutex;
};
struct threadData
{
    struct Page *pages[6];
    unsigned int randState;
    const char *threadName;
    int *finished;
};



/************************************************
 * Thread Function(s)
 * *********************************************/

void* threadUpdateBalance(void* arg)
{
    struct threadData *data = (struct threadData*)arg;
    
    char fileName[12];
    char readVal[10];
    char transType;
    float transAmount;

    data->randState = time(NULL) ^ getpid() ^ pthread_self();

    sprintf(fileName, "%s.in", data->threadName);
    FILE *inFile = fopen(fileName, "r");   

    while(fscanf(inFile, "%c", &transType) != EOF)
    {
        //TODO: Somewhere in here put code to request and modify pages
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
           printf("Account balance after thread %s is $%.2f\n", data->threadName, balance);
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
    
    (*data->finished) += 1;
    pthread_exit(NULL);
}

/***********************************************
 * Functions
 * ********************************************/

void createPages(struct Page **head)
{
    struct Page *curPage = (struct Page*)malloc(sizeof(struct Page));
    curPage->R = 0;
    curPage->M = 0;
    curPage->owner = -1;
    curPage->next = NULL;
    pthread_mutex_init(&curPage->pageMutex, NULL);
    (*head) = curPage;
    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        struct Page *newPage = (struct Page*)malloc(sizeof(struct Page));
        curPage->next = newPage;
        newPage->R = 0;
        newPage->M = 0;
        newPage->owner = -1;
        newPage->next = NULL;
        pthread_mutex_init(&newPage->pageMutex, NULL);
        curPage = newPage;
    }
    curPage->next = (*head);
}

void destroyPages(struct Page **head)
{
    //TODO: add pthread_mutex_destroy to loop
    struct Page *prevPage = (*head);
    struct Page *nextPage = prevPage->next;
    for (int i = 0; i < NUM_OF_THREADS-1; i++)
    {
        prevPage->next = NULL;
        free(prevPage);
        prevPage = nextPage;
        nextPage = nextPage->next;
    }
    prevPage->next = NULL;
    free(prevPage);
    nextPage->next = NULL;
    free(nextPage);    
}

void requestPage(struct Page **page)
{
    //TODO:  Code to handle page requests

}

/************************************************
 * Main
 * *********************************************/

int main (void)
{
    struct threadData data[NUM_OF_THREADS];
    struct Page *head = NULL;
    createPages(&head);
    struct Page *page = head;

    pthread_t thread[NUM_OF_THREADS];
    
    pthread_mutex_init(&mutex, NULL);
    
    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        data[i].threadName = threadNames[i];
        data[i].finished = &finished;
        pthread_create(&thread[i], NULL, threadUpdateBalance, (void*) &data[i]);
    }
    while (finished < 5)
    {
       //TODO: Code to randomly reset R in pages 
    }    
    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        pthread_join(thread[i], NULL);
    }
   
    pthread_mutex_destroy(&mutex);

    destroyPages(&head);
    printf("%d\n", finished);
    return 0;
}
