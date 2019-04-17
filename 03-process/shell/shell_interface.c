#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_LINE 80+1 // the max length command
#define MAX_ARGN MAX_LINE/2

const char * read_line(size_t size);
bool is_empty(const char *str);
int parse_line(char *str, char **args, int limit);

int main(void) {
		int argn;
		int wstatus;
    char line[MAX_LINE];
    char *args[MAX_ARGN + 1];  
    pid_t pid;

    while (true) {
        printf("jsh> ");  // jsh == jenny's shell, of course
        fflush(stdout);

				line = read_line(MAX_LINE);
				if (line == EOF || line[0] == '\0')
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

const char * read_line(size_t max_size)
{
		int tmp;
		char *buf
		size_t len;

		if (fgets(buf, max_size, stdin) == NULL)
				return EOF;

		if ((len = strlen(buf)) > 0) {
				if (buf[len - 1] == '\n') {
					buf[len - 1] = '\0';
				} 
				else {
						while(true){
								tmp = getchar();
								if (tmp == EOF || tmp == '\n')
										break;
						}
				}
		}

		return buf;
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
