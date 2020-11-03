#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <string.h>

int main(void)
{
    FILE* inFile = fopen("hw9.data", "r");
    FILE *outFile = fopen("noThread.out", "w");
    char readVal [100];

    while (fscanf(inFile, "%s\n", readVal) != EOF)
    {
        if (strlen(readVal) > 30)
        {
            printf("%s\n", readVal);
        }
        
        fprintf(outFile, "%s\t%d\n", readVal, 1);
    }
   

    return 0;
}
