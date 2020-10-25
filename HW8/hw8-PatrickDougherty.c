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
#define PF_PROB 15  //Page fault probabiilty, higher is more likely
#define R_RESET_MAX_TIME 500 //Max time interval in us
#define R_RESET_MIN_TIME 200 //Min time interval in us
/************************************************
 * Global Variables
 * *********************************************/
pthread_mutex_t mutex, pageMutex;
const char* threadNames[] = {"Vlad", "Frank", "Bigfoot", "Casper", "Gomez"};
float balance = 0.0f;
int finished = 0;
struct Page
{
    int R;
    int M;
    const char  *owner;  //owning treads name
    struct Page **ownerPtr; //Pointer to owning treads poonter to this page
    struct Page *next;
};
struct threadData
{
    struct Page *pages[NUM_OF_THREADS + 1];  //Pointers to owned pages
    struct Page **head;
    unsigned int randState; 
    const char *threadName;
    int *finished;
};

/***********************************************
 * Stubs
 * ********************************************/
void requestPage(struct threadData *data);

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
    //Create unique rand seed by XORing time, pid and tid
    data->randState = time(NULL) ^ getpid() ^ pthread_self();
    //Request initial page
    requestPage(data);

    //Generate file name and open file
    sprintf(fileName, "%s.in", data->threadName);
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
           printf("Account balance after thread %s is $%.2f\n", data->threadName, balance);
           int pagefault = 1;
           //Check if thread still owns any pages
           for (int i = 0; i < NUM_OF_THREADS+1; i++)
           {
               if (data->pages[i] != NULL)
               {
                   pagefault = 0;
                   break;
               }
           }
           //If out of pages, or randomly, generate a page fault
           if (((rand_r(&data->randState)%100) < PF_PROB) || pagefault)
           {
               printf("Page fault in thread %s\n", data->threadName);
               requestPage(data);
           }
           //Set M and R bits as directed
           for (int i = 0; i < NUM_OF_THREADS+1; i++)
           {
               if (data->pages[i] != NULL)
               {
                   if (balance < 0)
                   {
                       data->pages[i]->R = 1;
                       data->pages[i]->M = 1;
                   }
                   else
                       data->pages[i]->R = 1;
               }
           }
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

//Create a circular linked list of length NUM_OF_THREADS + 1
void createPages(struct Page **head)
{
    struct Page *curPage = (struct Page*)malloc(sizeof(struct Page));
    curPage->R = 0;
    curPage->M = 0;
    curPage->owner = "Morticia";
    curPage->ownerPtr = NULL;
    curPage->next = NULL;
    (*head) = curPage;
    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        struct Page *newPage = (struct Page*)malloc(sizeof(struct Page));
        curPage->next = newPage;
        newPage->R = 0;
        newPage->M = 0;
        newPage->owner = "Morticia";
        newPage->ownerPtr = NULL;
        newPage->next = NULL;
        curPage = newPage;
    }
    curPage->next = (*head);
}

//Destroy circular linked list
void destroyPages(struct Page **head)
{
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

//Handle page requests and update page ownership
void requestPage(struct threadData *data)
{
    struct Page *C0 = NULL, *C1 = NULL, *C2 = NULL, *C3 = NULL;
    pthread_mutex_lock(&pageMutex); //We need this because the main thread may reset R:e
    struct Page *curPage = (*data->head);
    //Find and select, if it exists, one page of each class
    for (int i = 0; i <= NUM_OF_THREADS; i++)
    {
        int class = 0;
        class += 2 * curPage->R;
        class += curPage->M;
        switch(class)
        {
            case 0:
                C0 = curPage;
                break;
            case 1:
                C1 = curPage;
                break;
            case 2:
                C2 = curPage;
                break;
            case 3:
                C3 = curPage;
                break;
        }
        curPage = curPage->next;
    }
    //Select page of lowest class
    if (C0 != NULL)
        curPage = C0;
    else if (C1 != NULL)
        curPage = C1;
    else if (C2 != NULL)
        curPage = C2;
    else if (C3 != NULL)
        curPage = C3;
    else
    {
        printf("Whoa!  We found no classifiable pages!?\n");
        pthread_mutex_unlock(&pageMutex);
        return;
    }
    //Announce selected page
    printf("Page selected for replacement. M:%d, R:%d, O: %s\n", curPage->M,
                                                                curPage->R,
                                                                curPage->owner);
    //Replace page ownership
    for (int i = 0; i <= NUM_OF_THREADS+1; i++)
    {
        //Find a empty slot in the page table
        if (data->pages[i] == NULL)
        {
            //Add pointer to page to page table
            data->pages[i] = curPage;
            //Check for previous owner
            if (curPage->ownerPtr != NULL)
                //Set previous owner's page table entry for this page to NULL
                (*curPage->ownerPtr) = NULL;
            //Store pointer to new owners page table entry for this page
            curPage->ownerPtr = &data->pages[i];
            //Reset M and R
            curPage->M = 0;
            curPage->R = 0;
            //Update owner name
            curPage->owner = data->threadName;
            //Change head so we don't start there again, make selection slightly more random
            (*data->head) = curPage->next->next;
            pthread_mutex_unlock(&pageMutex);
            return;
        }
    }
    printf("%s already has all the Pages! What a resource hog...\n", data->threadName);
    pthread_mutex_unlock(&pageMutex);
}

/************************************************
 * Main
 * *********************************************/

int main (void)
{
    struct threadData data[NUM_OF_THREADS];
    struct Page *head = NULL;
    createPages(&head);
    unsigned int randState = time(NULL) ^ getpid();
    int randVal;
    pthread_t thread[NUM_OF_THREADS];
    
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&pageMutex, NULL);
    
    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        
        data[i].threadName = threadNames[i];
        data[i].finished = &finished;
        data[i].head = &head;
        for (int j = 0; j <= NUM_OF_THREADS; j++)
            data[i].pages[j] = NULL;
        pthread_create(&thread[i], NULL, threadUpdateBalance, (void*) &data[i]);
    }
    //While monsters are spending randomly reset pages R bit
    while (finished < 5)
    {
        //Randomely wait sometime between R_RESET_MAX_TIME and R_RESET_MIN_TIME
        usleep(rand_r(&randState)%(R_RESET_MAX_TIME + 1 - R_RESET_MIN_TIME)+R_RESET_MIN_TIME);
        //printf("Reseting R's\n"); 
        pthread_mutex_lock(&pageMutex);
        //Reset R bits (...well ints really)
        struct Page *curPage = head;
        curPage->R = 0;
        for (int i = 0; i < NUM_OF_THREADS; i++)
        {
            curPage = curPage->next;
            curPage->R = 0;
        }
        pthread_mutex_unlock(&pageMutex);
    }    
    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        pthread_join(thread[i], NULL);
    }
   
    pthread_mutex_destroy(&mutex);

    destroyPages(&head);
    return 0;
}
