#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_LINE 80 // the max length command
#define MAX_ARGN MAX_LINE/2

int read_line(char *buf, size_t size);
int is_empty(const char *str);
int parse_line(char *str, char **args, int limit);

int main(void) {
    char line[MAX_LINE + 1];
    char *args[MAX_ARGN + 1];  // command line arguemnts
    int argn;
    pid_t pid;
	int wstatus;

    while (true) {
        printf("jsh> ");  // jsh == jenny's shell, of course
        fflush(stdout);

        if (read_line(line, MAX_LINE + 1) == EOF)
            break;

        if (is_empty(line))
            continue;

		if ((argn = parse_line(line, args, MAX_ARGN)) == 0)
			continue;

		if (strcmp(args[0], "exit") == 0) {
			break;
		} 
        else if ((pid = fork()) == 0) {
			if (execvp(args[0], args) == -1) {
				fprintf(stderr, "%s\n", strerror(errno));
				break;
			}
		} 
        else {
            waitpid(pid, &wstatus, WUNTRACED);
		}
    }

    return 0;
}

int read_line(char *buf, size_t size)
{
	size_t len;
	int tmp;

	if (fgets(buf, size, stdin) == NULL)
		return EOF;

	if ((len = strlen(buf)) > 0) {
		if (buf[len - 1] == '\n') {
			buf[len - 1] = '\0';
		} 
        else {
			while ((tmp = getchar()) != EOF && tmp != '\n')
				;
		}
	}

	return 0;
}

int is_empty(const char *str)
{
	for (const char *p = str; *p; p++)
		if (!isspace(*p))
			return 0;

	return 1;
}

int parse_line(char *str, char **args, int limit)
{
	int n = 0;
	int is_escape = false;
	bool is_quote = false;
	char *p, *q;

	args[n++] = str;

	for (p = q = str; *p; p++) {
		if (is_quote) {
			if (p[0] == '"') {
				is_quote = false;
				continue;
			}

			if (p[0] == '\\' && (p[1] == '"' || p[1] == '\\'))
				p++;
			*q++ = *p;
		} 
        else if (is_escape) {
			*q++ = *p;
			is_escape = false;
		} 
        else {
			switch (p[0]) {
			case '\\':
				is_escape = true;
				break;
			case '"':
				is_quote = true;
				break;
			case ' ':
			case '\t':
				if (q != str && q[-1] != '\0') {
					*q++ = '\0';
					args[n++] = q;
				}
				break;
			default:
				*q++ = *p;
			}
		}
	}

	*q = '\0';
	args[n] = NULL;

	return n;
}
