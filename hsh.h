#ifndef HSH_H
#define HSH_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

extern char **environ;

/* shell.c */
int run_shell(char *argv0);

/* parser.c */
char **split_line(char *line);
void free_tokens(char **tokens);

/* path.c */
char *find_in_path(char *cmd);

/* execute.c */
int execute_cmd(char **tokens, unsigned long ln, char *argv0, int *status);
void print_error(char *argv0, unsigned long ln, char *cmd, char *msg);

#endif /* HSH_H */
