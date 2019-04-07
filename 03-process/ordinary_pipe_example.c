#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 25
#define READ_END 0
#define WRITE_END 1

int main(void) {
    char write_msg[BUFFER_SIZE] = "Greetings.";
    char read_msg[BUFFER_SIZE];
    int fd[2];
    pid_t pid;

    // Create the pipe
    if (pipe(fd) == -1) {
        fprintf(stderr, "Pipe failed.");
        return 1;
    }

    // Fork a child process
    pid = fork();

    // parent process
    if (pid > 0) { 
        printf("In parent process. PID: %d", getpid());
        // Close the unused end of the pipe
        close(fd[READ_END]);

        // Write to the pipe
        write(fd[WRITE_END], write_msg, strlen(write_msg)+1);
        close(fd[WRITE_END]);
    }
    // child process
    else {
        printf("In child process. PID: %d", getpid());

        // Close the unused end of the pipe
        close(fd[WRITE_END]);

        // Read from the pipe
        read(fd[READ_END], read_msg, BUFFER_SIZE);
        printf("read %s", read_msg);
        close(fd[READ_END]);
    }

    return 0;
}
