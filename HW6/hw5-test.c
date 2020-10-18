/************************************************
 * CSci 451 - HW5 Test Program
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 30 September 2020
 * *********************************************/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int numOfChars = atoi(argv[1]);
    char a[numOfChars], b[numOfChars], c[numOfChars];

    //build test files
    FILE *inFile = fopen("hw5-1.in", "w");
    for (int j = 0; j < numOfChars; j++)
    {
        int num = (rand() % (122 - 97 + 1)) + 97;
        a[j] = (char)num;
        fprintf(inFile, "%c\n", a[j]);
    }
    fclose(inFile);

    inFile = fopen("hw5-2.in", "w");
    for (int j = 0; j < numOfChars; j++)
    {
        int num = (rand() % (57 - 48 + 1)) + 48;
        b[j] = (char)num;
        fprintf(inFile, "%c\n", b[j]);
    }
    fclose(inFile);

    inFile = fopen("hw5-3.in", "w");
    for (int j = 0; j < numOfChars; j++)
    {
        int num = (rand() % (47 - 33 + 1)) + 33;
        c[j] = (char)num;
        fprintf(inFile, "%c\n", c[j]);
    }
    fclose(inFile);

    //call program to be tested
    system("./a.out");

    //validate results
    char testChar;
    if((inFile = fopen("hw5.out", "r")) == NULL)
    {
        printf("Output file failed to open or be found\n");
        exit(0);
    }
    for (int i = 0; i < numOfChars; i++)
    {
        fscanf(inFile, "%c\n", &testChar);
        if (testChar != a[i])
        {
            printf("Char mismatch at %d in file 1", i);
            fclose(inFile);
            exit(0);
        }
        fscanf(inFile, "%c\n", &testChar);
        if (testChar != b[i])
        {
            printf("Char mismatch at %d in file 2", i);
            fclose(inFile);
            exit(0);
        }
        fscanf(inFile, "%c\n", &testChar);
        if (testChar != c[i])
        {
            printf("Char mismatch at %d in file 3", i);
            fclose(inFile);
            exit(0);
        }
    }
    printf("All characters where copied correctly\n");
    return 0;
}
