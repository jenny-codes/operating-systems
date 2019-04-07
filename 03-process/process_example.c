#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIZE 5

int nums[SIZE] = {0, 1, 2, 3, 4};

int main() {
    int i;
    pid_t pid;

    pid = fork();

    for (i = 0; i < SIZE; i++) {
        printf("ORIGINAL: %d \n", nums[i]);
    }

    if (pid == 0) {
        printf("IN CHILD PROCESS \n");

        for (i = 0; i < SIZE; i++) {
            nums[i] *=  -i;
            printf("CHILD: %d \n", nums[i]);
        }
    }
    else if (pid > 0) {
        printf("IN PARENT PROCESS \n");

        for (i = 0; i < SIZE; i++) {
            printf("PARENT: %d \n", nums[i]);
        }
        wait(NULL);
    } 
        
    return 0;
}

/* Whyyyyyyyyyyyyyyyy? */
/* When `wait()` is placed after the loop the child process is executed after parent */
/* When `wait()` is deleted or placed before the loop the child process is executed before parent. Even the "IN PARENT PROCESS" statement comes after child process */
