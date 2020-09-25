#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define NUM_OF_INTS 1000

int main(void)
{
    FILE *inFile = fopen("hw4.in", "w");
    for (int i = 0; i < NUM_OF_INTS; i++)
    {
        fprintf(inFile, "%d\n", rand());
    }
    fclose(inFile);

    system("./a.out");

    inFile = fopen("hw4.in", "r");
    FILE *outFile = fopen("hw4.out", "r");
    int inVal, outVal;
    int count = 0;
    while(fscanf(inFile, "%d\n", &inVal) != EOF)
    {
        if ((inVal % 2) == 0)
        {
            fscanf(outFile, "%d\n", &outVal);
            assert (outVal == inVal);
            fscanf(outFile, "%d\n", &outVal);
            assert (outVal == inVal);
        }
        else
        {
            fscanf(outFile, "%d\n", &outVal);
            assert (outVal == inVal);
        }
        count++;
    }
    fclose(inFile);
    fclose(outFile);
    printf("copied %d values without an error", count);
    return 0;
}
