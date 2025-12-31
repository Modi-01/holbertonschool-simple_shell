#ifndef HSH_H
#define HSH_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

extern char **environ;

/* globals used for exact error formatting */
extern char *g_argv0;
extern unsigned long g_ln;

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

/* string helpers */
size_t _strlen(const char *s);
int _strcmp(const char *s1, const char *s2);
int _strncmp(const char *s1, const char *s2, size_t n);
char *_strcpy(char *dest, const char *src);
char *_strchr(const char *s, int c);
char *_strcat(char *dest, const char *src);

/* env_builtins.c */
int builtin_setenv(char **tokens, int *status);
int builtin_unsetenv(char **tokens, int *status);

/* env.c */
int shell_setenv(const char *name, const char *value);
int shell_unsetenv(const char *name);

/* cleanup.c */
void cleanup_env(void);

/* ====== ADD THIS ====== */
int builtin_cd(char **tokens, int *status);
/* ====================== */

#endif /* HSH_H */
