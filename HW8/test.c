/************************************************
 * Csci 451 - Test Program for HW7
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 18 Oct 20
 * *********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXSIZE 10000
#define MAX_AMOUNT 100
#define EXIT_PROB 128 //larger is less likely to exit before reaching max size
#define MORE_N_PROB 4 //larger is more likely to put more N's in each RNW
#define PLUS_MINUS_PROB 2 //larger is more likely to generate a minus

const char* names[] = {"Vlad", "Frank", "Bigfoot", "Casper", "Gomez"};

void randomTrans(char* randTrans)
{
    char plusMinus;
    float amount;
    if (rand() < (RAND_MAX/PLUS_MINUS_PROB))
        plusMinus = '+';
    else
        plusMinus = '-';
    amount = (float)(((double)rand()/(double)(RAND_MAX)) * MAX_AMOUNT);
    sprintf(randTrans, "%c%.2f", plusMinus, amount);
    
}

int main(void)
{
    //Create randomly filled transation records
    char fileName[12];
    char transAmt[15];    
    srand(time(NULL));
    for (int i = 0; i < 5; i++)
    {
        int size = 0;
        sprintf(fileName, "%s.in", names[i]);
        FILE *f = fopen(fileName, "w");

        while (size < MAXSIZE)
        {
            fprintf(f, "R\n");
            randomTrans(transAmt);
            fprintf(f, "%s\n", transAmt);
            while (rand() > (RAND_MAX/MORE_N_PROB))
            {
                randomTrans(transAmt);
                fprintf(f, "%s\n", transAmt);
                size += 1;
            }
            fprintf(f, "W\n");
            size += 3;
            if (rand() < RAND_MAX/EXIT_PROB)
                break;
        }

        fclose(f);
    }
    
    //Sequentially total transaction records
    float balance = 0.0f;
    char transType;
    float transAmount;
    for (int i = 0; i < 5; i++)
    {
        sprintf(fileName, "%s.in", names[i]);
        FILE *inFile = fopen(fileName, "r");

        while(fscanf(inFile, "%c", &transType) != EOF) 
        { 
            if (transType == 'R') 
            { 
                fscanf(inFile, "\n"); 
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
               fscanf(inFile, "\n");
            }
            else
            {
                printf("Invalid transaction type encountered: %c\n", transType);
            }
        }
        fclose(inFile);
    }
    printf("Calculated balance should be: %.2f\n", balance);

    system("./a.out");

    return 0;
}
