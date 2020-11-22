/************************************************
 * CSci 451 - HW10, Program 0
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 16Nov20
 * *********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>    //pipe
#include <sys/types.h>  //waitpid
#include <sys/wait.h>   //waitpid
#include <sys/sem.h>    //semget, semctl, semop
#include <errno.h>      //errno
#include <unistd.h>     //pipe, close
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>

/************************************************
 * Global Variables
 * *********************************************/
union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};

/************************************************
 * Functions
 * *********************************************/
//TODO: Create a semaphore
int initsem(key_t key) {
    union semun arg;
    struct semid_ds buf;
    struct sembuf sb;
    int semid;
    
    semid = semget(key, 2, IPC_CREAT | IPC_EXCL | 0666);

    if (semid >= 0) {
        sb.sem_op = 1;
        sb.sem_flg = 0;
        arg.val = 1;
        for (sb.sem_num = 0; sb.sem_num < 2; sb.sem_num++) {
            if (semop(semid, &sb, 1) == -1) {
                int e = errno;
                semctl(semid, 0, IPC_RMID);
                errno = e;
                return -1;
            }
        }
        semctl(semid, 1, SETVAL, arg);
        arg.val = 0;
        semctl(semid, 0, SETVAL, arg);
    } 
    return semid;
}

/************************************************
 * Main (Driver Function)
 * *********************************************/

int main(int argc, char **argv) {
    //Check for valid number of arguments
    if (argc < 3 || argc > 3) {
        printf("Must have two arguments. Usage: program <Input File Name> <Output File Name>\n");
        return 0;
    }
    //Create necessary (more or less) variables
    char *inFileName = argv[1];
    //Check if input file exists
    FILE *file;
    if ((file = fopen(inFileName, "r")) == NULL) {
        printf("Input file does not exist!\n");
        return 0;
    }
    fclose(file);
    char *outFileName = argv[2];
    char *shmFilePath = "HW11shm";
    key_t semKey1, semKey2;
    int semid1, semid2, status, shm_fd, pipe12[2], pipe23[2];
    pid_t pid1, pid2, pid3;
    void *shmid;
    semKey1 = ftok("input.data", 'A');
    semKey2 = ftok("input.data", 'B');
    //Create pipes
    pipe(pipe12);
    pipe(pipe23);
    //Create, truncate, and map shared memory
    if ((shm_fd = shm_open(shmFilePath, O_CREAT | O_RDWR, 0666)) == -1) {
        perror("shm_open");
        return 0;
    }
    ftruncate(shm_fd, 4096);
    if ((shmid = mmap(0, 4096, PROT_WRITE | PROT_READ,
                    MAP_SHARED, shm_fd, 0)) == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        return 0;
    }
    //Create semaphores
    if ((semid1 = initsem(semKey1)) == -1) {
        perror("initsem1");
        munmap(shmid, 4096);
        shm_unlink(shmFilePath);
        return 0;
    }
    if ((semid2 = initsem(semKey2)) == -1) {
        perror("initsem2");
        semctl(semid1, 0, IPC_RMID);
        munmap(shmid, 4096);
        shm_unlink(shmFilePath);
        return 0;
    }
    //Convert sem IDs to char*
    char pipe12Read[3], pipe12Write[3];
    char pipe23Read[3], pipe23Write[3];
    char sem1Char[10], sem2Char[10], shmChar[17];
    sprintf(sem1Char, "%d", semid1);
    sprintf(sem2Char, "%d", semid2);
    printf("Zucca Gigantopithicus is here to translate your file.\n");
    //Start program1
    if ((pid1 = fork()) == 0) {
        close(pipe23[0]);
        close(pipe23[1]);
        close(pipe12[0]);
        sprintf(pipe12Write, "%d", pipe12[1]);
        char *args[] = {"./program1", inFileName, pipe12Write, sem1Char, NULL};
        execv(args[0], args);
    }
    //Start program2
    if ((pid2 = fork()) == 0) {
        close(pipe12[1]);
        close(pipe23[0]);
        sprintf(pipe12Read, "%d", pipe12[0]);
        sprintf(pipe23Write, "%d", pipe23[1]);
        char *args[] = {"./program2", pipe12Read, pipe23Write, sem1Char,
                        sem2Char, shmFilePath, NULL};
        execv(args[0], args);
    }
    //Close pipe12 to prevent it from being in the memory of program3
    close(pipe12[0]);
    close(pipe12[1]);
    //Start program3 
    if ((pid3 = fork()) == 0) {
        close(pipe23[1]);
        sprintf(pipe23Read, "%d", pipe23[0]);
        char *args[] = {"./program3", pipe23Read, sem2Char, shmFilePath, outFileName, NULL};
        execv(args[0], args);
    }
    //close pipes in program0, its not using them
    close(pipe23[0]);
    close(pipe23[1]);
    //Wait for children
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
    waitpid(pid3, &status, 0);
    //Clean up shared memory and semaphores
    //munmap(shmid, 4096);
    shm_unlink(shmFilePath);
    semctl(semid1, 0, IPC_RMID);
    semctl(semid2, 0, IPC_RMID);
    printf("ouryay ilefay avehay eenbay ranslatedtay =)\n");
    return 0;
}
        
