/* reference: "https://github.com/dalmia/Operating-Systems" */

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

typedef int bool;
#define true 1
#define false 0

#define RL_BUFSIZE 1024 /* buffer size for reading user input */
#define TOK_BUFSIZE 64 /* buffer size for splitting the arguments */
#define HIST_SIZE 10 /* buffer size for storing history of commands */
#define TOK_DELIM " \t\r\n\a" /* delimiters for parsing the arguments */

/* global variable to check parent and child process concurrency */
bool is_concurrent = false;
/* global variable to point to the last command executed */
int cur_pos = -1;
/* global variable storing the history of commands executed */
char *history[HIST_SIZE];
int cur_bufsize = TOK_BUFSIZE;

/* Function declarations for built-in shell commands */
int jsh_cd(char **args);
int jsh_help(char **args);
int jsh_exit(char **args);

/* List of built-in commands, followed by their corresponding functions */
char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &jsh_cd,
    &jsh_help,
    &jsh_exit
};

int lsh_num_builtins(){
    return sizeof(builtin_str) / sizeof(char *);
}
