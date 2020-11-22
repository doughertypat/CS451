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
#include <fcntl.h>
#include <sys/mman.h>

/************************************************
 * Main
 * *********************************************/

int main(int argc, char **argv) {
    int type1, type2;
    char buf[100] = { '\0' };
    int pipe23[2], semid, shm_fd;
    int *shmid;
    void *shm;
    struct sembuf sem_buf;

    pipe23[0] = atoi(argv[1]);
    semid = atoi(argv[2]);
    //Open shared memory
    if((shm_fd = shm_open(argv[3], O_RDONLY, 0)) == -1)
        perror("C3 shm_open\n");
    if((shm = mmap(0, 4096, PROT_READ, MAP_SHARED, shm_fd, 0)) == MAP_FAILED)
            perror("C3 mmap\n");
    //We'll be reading int's so lets cast to an int ptr
    shmid = (int *)shm;
    FILE *outFile = fopen(argv[4], "w");

    sem_buf = (struct sembuf){0, -1, 0};
    semop(semid, &sem_buf, 1);
    read(pipe23[0], &buf, 100);
    sem_buf = (struct sembuf){1, 1, 0};
    semop(semid, &sem_buf, 1);
    fprintf(outFile, "%s", buf); 
    while (1) {
        sem_buf = (struct sembuf){0, -1, 0};
        semop(semid, &sem_buf, 1);
        if (read(pipe23[0], &buf, 100) < 1) {
            sem_buf = (struct sembuf){1, 1, 0};
            semop(semid, &sem_buf, 1);
            break;
        }
        sem_buf = (struct sembuf){1, 1, 0};
        semop(semid, &sem_buf, 1);
        fprintf(outFile, " %s", buf);
    }
    
    close(pipe23[0]);
    fclose(outFile);
    type1 = shmid[0];
    printf("Type 1: %d\n", type1);
    type2 = shmid[1]; 
    printf("Type 2: %d\n", type2);
    return 0;
}

