/************************************************
 * CSci 451 - HW10, Program 3
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 16 Nov 20
 * *********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>

int main(int argc, char **argv) {
    char buf[50];
    int pipe23[2];

    pipe23[0] = atoi(argv[1]);
    pipe23[1] = atoi(argv[2]);
    close(pipe23[1]);
    printf("3 before loop\n");
    /*
    while (read(pipe23[0], &buf, 50) > 0) {
        printf("%s ", buf);
    }
    */
    printf("3 after loop\n");
    close(pipe23[0]);
    printf("3 done\n");
    return 0;
}

