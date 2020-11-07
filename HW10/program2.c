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
    char buf[100] = { '\0' };
    int pipe12[2], pipe23[2];
    int semid, res;
    int stringLen, type1 = 0, type2 = 0;
    struct sembuf sem_unlock, sem_lock;
    //Set-up sembufs for 'locking' and 'unlocking' the semaphore
    sem_unlock.sem_num = 0;
    sem_unlock.sem_op = 1;
    sem_unlock.sem_flg = 0;
    sem_lock.sem_num = 0;
    sem_lock.sem_op = -1;
    sem_lock.sem_flg = 0;
    //Convert pipe and sem IDs back to int's
    pipe12[0] = atoi(argv[1]);
    pipe12[1] = atoi(argv[2]);
    semid = atoi(argv[3]);
    pipe23[0] = atoi(argv[4]);
    pipe23[1] = atoi(argv[5]);
    //make pipe12 non-blocking on read
    fcntl(pipe12[0], F_SETFL, O_NONBLOCK);
    //close unused ends of pipes
    close(pipe12[1]);
    close(pipe23[0]);
    while (1) {
        semop(semid, &sem_lock, 1);
        //Check if read returns EOF or error
        if ((res = read(pipe12[0], &buf, 100)) <= 0) {
            //Check if EOF reached
            if (res == 0) {
                semop(semid, &sem_unlock, 1);
                break;
            //Read would've been blocked so release semaphore and continue
            } else if (errno == EAGAIN) {
                semop(semid, &sem_unlock, 1);
                continue;
            }
        }
        semop(semid, &sem_unlock, 1);
        //Pig latin magical converter
        stringLen = strlen(buf);
        if (isVowel(buf[0])) {
            type1 += 1;
            if (isLetter(buf[stringLen - 1]))
                strcat(buf, "ray");
            else {
                //Per specification only , or . will be encountered
                if (buf[stringLen - 1] == ',') {
                    buf[stringLen - 1] = '\0';
                    strcat(buf, "ray,");
                } else {
                    buf[stringLen - 1] = '\0';
                    strcat(buf, "ray.");
                }
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
                strcpy(buf2, buf+1);
                buf2[stringLen - 2] = buf[0];
                if (buf[stringLen - 1] == ',')
                    strcat(buf2, "ay,");
                else
                    strcat(buf2, "ay.");
                strcpy(buf, buf2);
            }
        }
        write(pipe23[1], &buf, 100);
    }
    //Close pipe12 and sem cuz we're done with them
    close(pipe12[0]);
    semctl(semid, 0, IPC_RMID);
    //Write type count to files... close pipe after writing to files
    FILE *out1 = fopen("shared1.dat", "w");
    FILE *out2 = fopen("shared2.dat", "w");
    fprintf(out1, "%d\n", type1);
    fprintf(out2, "%d\n", type2);
    fclose(out1);
    fclose(out2);
    //Close pipe here so program3 will be released from read loop
    close(pipe23[1]);

    return 0;
}
