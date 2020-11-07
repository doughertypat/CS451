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
    FILE *inFile = fopen("input.data", "r");
    char buf[100];
    int pipe12[2];
    int semid;
    struct sembuf sem_unlock, sem_lock;
    //Set-up sembufs for 'locking' and 'unlocking' the semaphore
    sem_unlock.sem_num = 0;
    sem_unlock.sem_op = 1;
    sem_unlock.sem_flg = 0;
    sem_lock.sem_num = 0;
    sem_lock.sem_op = -1;
    sem_lock.sem_flg = 0;
    //Convert pipe and sem ID's to int's
    semid = atoi(argv[3]);
    pipe12[0] = atoi(argv[1]);
    pipe12[1] = atoi(argv[2]);
    //Make the pipe non-blocking on write
    fcntl(pipe12[1], F_SETFL, O_NONBLOCK);
    close(pipe12[0]);
    fscanf(inFile, "%s", buf);
    while(1) {
        if (semop(semid, &sem_lock, 1) == -1) {
            perror("semop-mutex_lock");
            exit(EXIT_FAILURE);
        }
        //If the pipe is full then release the semaphore and continue
        if (write(pipe12[1], &buf, 100) < 0) {
            semop(semid, &sem_unlock, 1);
            continue;
        }
        //If the write was successful then release the semaphore
        // and read a new word from the file
        semop(semid, &sem_unlock, 1);
        if (fscanf(inFile, "%s", buf) == EOF)
            break;
    }
    if (close(pipe12[1]) == -1) 
        perror("pipe_close");
    fclose(inFile);
    exit(EXIT_SUCCESS);
}
