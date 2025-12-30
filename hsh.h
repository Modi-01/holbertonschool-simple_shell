#ifndef HSH_H
#define HSH_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

extern char **environ;

/* shell.c */
int run_shell(char *argv0);

/* parser.c */
char **split_line(char *line);
void free_tokens(char **tokens);

/* path.c */
char *resolve_path(char *cmd);

/* execute.c */
int execute_tokens(char **tokens, char *argv0, unsigned long ln, int *status);
void print_error(char *argv0, unsigned long ln, char *cmd, char *msg);

/* builtins.c */
int handle_builtins(char **tokens, int *status);

/* getline.c */
ssize_t _getline(char **lineptr, size_t *n, int fd);

size_t _strlen(const char *s);
int _strcmp(const char *s1, const char *s2);
char *_strcpy(char *dest, const char *src);
char *_strchr(const char *s, int c);

#endif /* HSH_H */
