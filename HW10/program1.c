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

int main(int argc, char **argv) {
    FILE *inFile = fopen("input.data", "r");
    char buf[50];
    int pipe12[2];
    int semid;
    struct sembuf sem_unlock, sem_lock;

    sem_unlock.sem_num = 0;
    sem_unlock.sem_op = 1;
    sem_unlock.sem_flg = 0;
    sem_lock.sem_num = 0;
    sem_lock.sem_op = -1;
    sem_lock.sem_flg = 0;
    
    pipe12[0] = atoi(argv[1]);
    pipe12[1] = atoi(argv[2]);
    semid = atoi(argv[3]);
    close(pipe12[0]);
    printf("1 before loop\n");
    while (fscanf(inFile, "%s", buf) != EOF) {
        printf("1 before critical region\n");
        if (semop(semid, &sem_lock, 1) == -1) {
            perror("semop-mutex_lock");
            exit(EXIT_FAILURE);
        }
        printf("1 in critical region\n");
        write(pipe12[1], &buf, 50);
        semop(semid, &sem_unlock, 1);
        printf("1 left critical region, sent: %s\n", buf);
    }
    printf("1 after loop\n");
    write(pipe12[1], '\0', 1);
    //close(pipe12[1]);
    printf("1 done\n");
    exit(EXIT_SUCCESS);
}
