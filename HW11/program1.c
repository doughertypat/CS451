/************************************************
 * CSci 451 - HW10, Program 1
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 16Nov20
 * *********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>

/************************************************
 * Main
 * *********************************************/

int main(int argc, char **argv) {
    FILE *inFile; 
    char buf[100];
    int pipe12[2];
    int semid;
    struct sembuf sem_buf;
    //Convert arguments to variables
    semid = atoi(argv[3]);
    pipe12[1] = atoi(argv[2]);
    inFile = fopen(argv[1], "r");
    while(fscanf(inFile, "%s", buf) != EOF) {
        sem_buf = (struct sembuf){1, -1, 0};
        semop(semid, &sem_buf, 1);
        write(pipe12[1], &buf, 100);
        sem_buf = (struct sembuf){0, 1, 0};
        semop(semid, &sem_buf, 1);
    }
    sem_buf = (struct sembuf){0, 1, 0};
    semop(semid, &sem_buf, 1);
    if (close(pipe12[1]) == -1) 
        perror("pipe_close");
    fclose(inFile);
    exit(EXIT_SUCCESS);
}
