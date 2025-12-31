#ifndef HSH_H
#define HSH_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>

#define READ_BUF_SIZE 1024

extern char **environ;

/**
 * struct shell_ctx - execution context
 * @av0: program name (argv[0])
 * @line_num: current command line number
 * @interactive: 1 if interactive, 0 otherwise
 * @should_exit: 1 if shell must exit, 0 otherwise
 * @exit_status: status code to exit with
 * @env: owned copy of environment (NULL if not initialized)
 */
typedef struct shell_ctx
{
	char *av0;
	unsigned long line_num;
	int interactive;
	int should_exit;
	int exit_status;
	char **env;
} shell_ctx_t;

void shell_loop(shell_ctx_t *ctx);

char **split_args(char *line);
void free_args(char **argv);
int execute_argv(shell_ctx_t *ctx, char **argv);

int handle_builtins(shell_ctx_t *ctx, char **argv);

ssize_t my_getline(char **lineptr, size_t *n, int fd);

void sigint_handler(int sig);

/* Environment management */
int env_init(shell_ctx_t *ctx);
void env_free(shell_ctx_t *ctx);
int env_set(shell_ctx_t *ctx, const char *name, const char *value);
int env_unset(shell_ctx_t *ctx, const char *name);

#endif /* HSH_H */
