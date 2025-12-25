#include "hsh.h"

static void child_exec(char **tokens, unsigned long ln, char *argv0);
static void parent_wait(pid_t pid, char *cmd, unsigned long ln,
		char *argv0, int *status);

/**
 * print_error - print error message in sh-like format
 * @argv0: program name
 * @ln: line number
 * @cmd: command name
 * @msg: message
 */
void print_error(char *argv0, unsigned long ln, char *cmd, char *msg)
{
	fprintf(stderr, "%s: %lu: %s: %s\n", argv0, ln, cmd, msg);
}

/**
 * execute_cmd - fork and execute command
 * @tokens: tokenized command
 * @ln: line number
 * @argv0: program name
 * @status: pointer to last status
 *
 * Return: 0 on success path, -1 on fork error
 */
int execute_cmd(char **tokens, unsigned long ln, char *argv0, int *status)
{
	pid_t pid;

	pid = fork();
	if (pid == -1)
	{
		print_error(argv0, ln, tokens[0], "fork failed");
		*status = 2;
		return (-1);
	}

	if (pid == 0)
		child_exec(tokens, ln, argv0);

	parent_wait(pid, tokens[0], ln, argv0, status);
	return (0);
}

/**
 * child_exec - execute command in child process
 * @tokens: tokenized command
 * @ln: line number
 * @argv0: program name
 */
static void child_exec(char **tokens, unsigned long ln, char *argv0)
{
	char *resolved = NULL;

	if (strchr(tokens[0], '/'))
		execve(tokens[0], tokens, environ);

	resolved = find_in_path(tokens[0]);
	if (!resolved)
	{
		print_error(argv0, ln, tokens[0], "not found");
		exit(127);
	}

	execve(resolved, tokens, environ);
	print_error(argv0, ln, tokens[0], strerror(errno));
	free(resolved);
	exit(errno == EACCES ? 126 : 127);
}

/**
 * parent_wait - wait for child and set status
 * @pid: child pid
 * @cmd: command name
 * @ln: line number
 * @argv0: program name
 * @status: pointer to last status
 */
static void parent_wait(pid_t pid, char *cmd, unsigned long ln,
		char *argv0, int *status)
{
	int wstatus;

	if (waitpid(pid, &wstatus, 0) == -1)
	{
		print_error(argv0, ln, cmd, "waitpid failed");
		*status = 2;
		return;
	}

	if (WIFEXITED(wstatus))
		*status = WEXITSTATUS(wstatus);
	else
		*status = 2;
}
