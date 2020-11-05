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
    
    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);

    if (semid >= 0) {
        sb.sem_op = 1;
        sb.sem_flg = 0;
        arg.val = 1;
        if (semop(semid, &sb, 1) == -1) {
            int e = errno;
            semctl(semid, 0, IPC_RMID);
            errno = e;
            return -1;
        }
        semctl(semid, 0, SETVAL, arg);
    } 
    return semid;
}
        

//TODO: create two pipes


/************************************************
 * Main (Driver Function)
 * *********************************************/
//TODO: Spawn 3 children
//  Child 1 and 2 must start at the same time
//  Child 3 can start after 1 and 2 terminate
int main(int argc, char **argv) {
    key_t semKey;
    int semid, status;
    int pipe12[2], pipe23[2];
    pid_t pid1, pid2;    
    semKey = ftok("input.data", 'A');

    pipe(pipe12);
  //  fcntl(pipe12[0], F_SETFL, O_NONBLOCK);
    pipe(pipe23);

    if ((semid = initsem(semKey)) == -1) {
        perror("initsem");
        return 0;
    }

    char pipe12Read[3], pipe12Write[3];
    char pipe23Read[3], pipe23Write[3];
    char semChar[10];
    sprintf(pipe12Read, "%d", pipe12[0]);
    sprintf(pipe12Write, "%d", pipe12[1]);
    sprintf(pipe23Read, "%d", pipe23[0]);
    sprintf(pipe23Write, "%d", pipe23[1]);
    sprintf(semChar, "%d", semid);
    if ((pid1 = fork()) == 0) {
        char *args[] = {"./program1", pipe12Read, pipe12Write, semChar, NULL};
        execv(args[0], args);
    }
    if ((pid2 = fork()) == 0) {
        char *args[] = {"./program2", pipe12Read, pipe12Write, semChar,
                        pipe23Read, pipe23Write, NULL};
        execv(args[0], args);
    }
    /*
    //TODO: Finish passing args for pipe23.
    if (fork() == 0) {
        char *args[] = {"./program3", pipe23Read, pipe23Write, NULL};
        execv(args[0], args);
    }
    */
    //close pipes in program0, its not using them
    close(pipe12[0]);
    close(pipe12[1]);
    close(pipe23[0]);
    close(pipe23[1]);
    //need to wait for program 1 to finish before destroying semephore
    printf("0 waiting on 1 and 2\n");
    waitpid(pid1, &status, 0);
    //close(pipe12[0]);
    //close(pipe12[1]);
    printf("0 done waiting on 1\n");
    waitpid(pid2, &status, 0);
    printf("0 done waiting on 2\n");
    semctl(semid, 0, IPC_RMID);
    return 0;
}
        
