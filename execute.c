#include "hsh.h"
#include <stdio.h>

/**
 * print_error - print error like /bin/sh
 * @argv0: program name
 * @ln: line number
 * @cmd: command
 * @msg: message
 */
void print_error(char *argv0, unsigned long ln, char *cmd, char *msg)
{
	fprintf(stderr, "%s: %lu: %s: %s\n", argv0, ln, cmd, msg);
}

/**
 * execute_tokens - fork/exec and set status
 * @tokens: argv
 * @argv0: program name
 * @ln: line number
 * @status: status pointer
 *
 * Return: 0 always
 */
int execute_tokens(char **tokens, char *argv0, unsigned long ln, int *status)
{
	pid_t pid;
	int wstatus;
	char *path = NULL;
	void (*old_handler)(int);

	if (!tokens || !tokens[0])
		return (0);

	if (_strchr(tokens[0], '/'))
		path = tokens[0];
	else
		path = resolve_path(tokens[0]);

	if (!path)
	{
		print_error(argv0, ln, tokens[0], "not found");
		*status = 127;
		return (0);
	}

	pid = fork();
	if (pid == -1)
	{
		if (path != tokens[0])
			free(path);
		*status = 1;
		return (0);
	}

	if (pid == 0)
	{
		/* Child: Ctrl+C should terminate the command */
		signal(SIGINT, SIG_DFL);

		execve(path, tokens, environ);

		if (errno == EACCES)
			print_error(argv0, ln, tokens[0], "Permission denied");
		else
			print_error(argv0, ln, tokens[0], "not found");

		if (path != tokens[0])
			free(path);

		_exit((errno == EACCES) ? 126 : 127);
	}

	/* Parent: ignore SIGINT while waiting (avoid double prompt) */
	old_handler = signal(SIGINT, SIG_IGN);
	waitpid(pid, &wstatus, 0);
	signal(SIGINT, old_handler);

	if (path != tokens[0])
		free(path);

	if (WIFEXITED(wstatus))
		*status = WEXITSTATUS(wstatus);
	else if (WIFSIGNALED(wstatus))
		*status = 128 + WTERMSIG(wstatus);

	return (0);
}

