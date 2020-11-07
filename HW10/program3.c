/************************************************
 * CSci 451 - HW10, Program 3
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 16 Nov 20
 * *********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>

/************************************************
 * Main
 * *********************************************/

int main(int argc, char **argv) {
    FILE *outFile = fopen("output.data", "w");
    FILE *type1, *type2;
    char buf[100];
    int pipe23[2];

    pipe23[0] = atoi(argv[1]);
    pipe23[1] = atoi(argv[2]);
    close(pipe23[1]);
   
    read(pipe23[0], &buf, 100);
    fprintf(outFile, "%s", buf); 
    while (read(pipe23[0], &buf, 100) > 0) {
        fprintf(outFile, " %s", buf);
    }
    
    close(pipe23[0]);
    fclose(outFile);
    type1 = fopen("shared1.dat", "r");
    fscanf(type1, "%s", buf);
    printf("Type 1: %s\n", buf);
    fclose(type1);
    type2 = fopen("shared2.dat", "r");
    fscanf(type2, "%s", buf);
    printf("Type 2: %s\n", buf);
    fclose(type2);
    return 0;
}

