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

int main(int argc, char **argv) {
    char buf[50] = { '\0' };
    int pipe12[2], pipe23[2];
    int semid, res;
    int stringLen, type1 = 0, type2 = 0;
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
    //pipe23[0] = atoi(argv[4]);
    //pipe23[1] = atoi(argv[5]);
    fcntl(pipe12[0], F_SETFL, O_NONBLOCK);
    //TODO: create pipe23 variables and close read end
    close(pipe12[1]);
    //close(pipe23[0]);
    printf("2 before loop\n");
    while (1) {
        semop(semid, &sem_lock, 1);
        if ((res = read(pipe12[0], &buf, 50)) <= 0) {
            if (errno == EAGAIN) {
                semop(semid, &sem_unlock, 1);
                sleep(1);
                continue;
            } else if (res == 0) {
                semop(semid, &sem_unlock, 1);
                printf("2 left critical region on to break\n");
                break;
            }
        }
        printf("2 leaving critical region\n");
        semop(semid, &sem_unlock, 1);
        printf("2 left critical region\n");
        stringLen = strlen(buf);
        if (isVowel(buf[0])) {
            type1 += 1;
            if (isLetter(buf[stringLen - 1]))
                strcat(buf, "ray");
            else {
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
            char buf2[50] = { '\0' }; 
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
        printf("%s ", buf);
        //write(pipe23[1], &buf, 50);
    }
    close(pipe12[0]);
    
    
    //TODO: Write type count to files BE CAREFUL about p3 reading to early
    printf("2 opening files\n");
    FILE *out1 = fopen("shared1.dat", "w");
    FILE *out2 = fopen("shared2.dat", "w");
    fprintf(out1, "%d\n", type1);
    fprintf(out2, "%d\n", type2);
    fclose(out1);
    fclose(out2);
    printf("2 closed files\n");
    //write(pipe23[1], "#*#*#", 50);
    //close(pipe23[1]);
    printf("2 done\n");
    

    return 0;
}
