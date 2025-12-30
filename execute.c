#include "hsh.h"

static char *resolve_cmd(char *cmd, int *code, char *argv0, unsigned long ln);
static void child_exec(char *path, char **tokens, char *argv0, unsigned long ln);
static void parent_wait(pid_t pid, int *status);

/**
 * print_error - print sh-like error: argv0: ln: cmd: msg
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
 * execute_tokens - execute command with PATH handling
 * @tokens: tokenized command
 * @argv0: program name
 * @ln: line number
 * @status: pointer to last status
 *
 * Return: 0 always (status updated)
 */
int execute_tokens(char **tokens, char *argv0, unsigned long ln, int *status)
{
	char *path;
	pid_t pid;
	int code = 0;

	path = resolve_cmd(tokens[0], &code, argv0, ln);
	if (!path)
	{
		*status = code;
		return (0);
	}

	pid = fork();
	if (pid == -1)
	{
		free(path);
		*status = 2;
		return (0);
	}

	if (pid == 0)
		child_exec(path, tokens, argv0, ln);

	parent_wait(pid, status);
	free(path);
	return (0);
}

/**
 * resolve_cmd - resolve command path and ensure it exists before fork
 * @cmd: command token
 * @code: output error code (127/126)
 * @argv0: program name
 * @ln: line number
 *
 * Return: malloc'd path to exec or NULL
 */
static char *resolve_cmd(char *cmd, int *code, char *argv0, unsigned long ln)
{
	char *path;

	*code = 0;
	if (!cmd || cmd[0] == '\0')
		return (NULL);

	if (_strchr(cmd, '/'))
	{
		if (access(cmd, F_OK) != 0)
		{
			print_error(argv0, ln, cmd, "not found");
			*code = 127;
			return (NULL);
		}
		if (access(cmd, X_OK) != 0)
		{
			print_error(argv0, ln, cmd, "Permission denied");
			*code = 126;
			return (NULL);
		}

		path = malloc(_strlen(cmd) + 1);
		if (!path)
		{
			*code = 2;
			return (NULL);
		}
		_strcpy(path, cmd);
		return (path);
	}

	path = resolve_path(cmd);
	if (!path)
	{
		print_error(argv0, ln, cmd, "not found");
		*code = 127;
	}
	return (path);
}

/**
 * child_exec - execve in child
 * @path: resolved path (malloc'd)
 * @tokens: argv for execve
 * @argv0: program name
 * @ln: line number
 */
static void child_exec(char *path, char **tokens, char *argv0, unsigned long ln)
{
	execve(path, tokens, environ);

	if (errno == EACCES)
		print_error(argv0, ln, tokens[0], "Permission denied");
	else
		print_error(argv0, ln, tokens[0], "not found");

	exit(errno == EACCES ? 126 : 127);
}

/**
 * parent_wait - wait for child and update status
 * @pid: child pid
 * @status: pointer to last status
 */
static void parent_wait(pid_t pid, int *status)
{
	int wstatus;

	if (waitpid(pid, &wstatus, 0) == -1)
	{
		*status = 2;
		return;
	}

	if (WIFEXITED(wstatus))
		*status = WEXITSTATUS(wstatus);
	else
		*status = 2;
}
