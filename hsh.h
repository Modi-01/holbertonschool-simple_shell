#ifndef HSH_H
#define HSH_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <stddef.h>

extern char **environ;

/* Globals used by some builtins/errors */
extern char *g_argv0;
extern unsigned long g_ln;

/* ===== String helpers (your custom functions) ===== */
size_t _strlen(const char *s);
char *_strcpy(char *dest, const char *src);
int _strncmp(const char *s1, const char *s2, size_t n);
char *_strchr(const char *s, int c);

/* ===== Errors ===== */
void print_error(const char *argv0, unsigned long ln,
		 const char *cmd, const char *msg);

/* ===== PATH / execution ===== */
char *resolve_path(char *cmd);

/* ===== Environment core ===== */
int shell_setenv(char *name, char *value);
int shell_unsetenv(char *name);

/* ===== Env builtins ===== */
int builtin_setenv(char **tokens);
int builtin_unsetenv(char **tokens);
void print_env_stdout(void);

/* ===== Ctrl+C ===== */
void sigint_handler(int sig);

#endif /* HSH_H */
