/************************************************
 * CSci 451 - HW10, Program 2
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
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>

/************************************************
 * Functions
 * *********************************************/

int isVowel(char t) {
    if (t == 'a' || t == 'e' || t == 'i' || t == 'o' || t == 'u'||
        t == 'A' || t == 'E' || t == 'I' || t == 'O' || t == 'U')
        return 1;
    return 0;
}

int isLetter(char d) {
    if (((d >= 'a') && (d <= 'z')) || ((d >= 'A') && (d <= 'Z')))
        return 1;
    return 0;
}

/************************************************
 * Main
 * *********************************************/

int main(int argc, char **argv) {
    char buf[100] = { '\0' }, tempChar[2] = { '\0' };
    int pipe12[2], pipe23[2];
    int semid1, semid2, res, shm_fd;
    int stringLen, type1 = 0, type2 = 0;
    struct sembuf sem_buf;
    int *shmid;
    void *shm;
    //Convert pipe and sem IDs back to int's
    pipe12[0] = atoi(argv[1]);
    semid1 = atoi(argv[3]);
    pipe23[1] = atoi(argv[2]);
    semid2 = atoi(argv[4]);
    //Open shared memory
    if ((shm_fd = shm_open(argv[5], O_RDWR, 0)) == -1)
        perror("C2 shm_open\n");
    if ((shm = mmap(0, 4096, PROT_WRITE, MAP_SHARED, shm_fd, 0)) == MAP_FAILED) 
        perror("mmap");
    //We are only writting int's so lets cast to an int ptr
    shmid = (int *)shm;
    //Do work
    while (1) {
        sem_buf = (struct sembuf){0, -1, 0};
        semop(semid1, &sem_buf, 1);
        //Check if read returns EOF
        if ((res = read(pipe12[0], &buf, 100)) < 1) {
            sem_buf = (struct sembuf){0, 1, 0};
            semop(semid2, &sem_buf, 1);
            break;
        }
        sem_buf = (struct sembuf){1, 1, 0};
        semop(semid1, &sem_buf, 1);
        //Pig latin magical converter
        stringLen = strlen(buf);
        if (isVowel(buf[0])) {
            type1 += 1;
            if (isLetter(buf[stringLen - 1]))
                strcat(buf, "ray");
            else {
                tempChar[0] = buf[stringLen-1];
                buf[stringLen-1] = '\0';
                strcat(buf, "ray");
                strcat(buf, tempChar);
            }
        } else {
            type2 += 1;
            char buf2[100] = { '\0' }; 
            if (isLetter(buf[stringLen -1])) {
                strcpy(buf2, buf+1);
                buf2[stringLen - 1] = buf[0];
                strcat(buf2, "ay");
                strcpy(buf, buf2);
            } else {
                tempChar[0] = buf[stringLen - 1];
                buf[stringLen - 1] = '\0';
                strcpy(buf2, buf+1);
                buf2[stringLen - 2] = buf[0];
                strcat(buf2, "ay");
                strcat(buf2, tempChar);
                strcpy(buf, buf2);
            }
        }
        sem_buf = (struct sembuf){1, -1, 0};
        semop(semid2, &sem_buf, 1);
        write(pipe23[1], &buf, 100);
        sem_buf = (struct sembuf){0, 1, 0};
        semop(semid2, &sem_buf, 1);
    }
    //Close pipe12 and sem cuz we're done with them
    close(pipe12[0]);
    //Write type count to files... close pipe after writing to files
    shmid[0] = type1;
    shmid[1] = type2;
    //Close pipe here so program3 will be released from read loop
    close(pipe23[1]);

    return 0;
}
