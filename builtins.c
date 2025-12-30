#include "hsh.h"

static void print_env(void);

/**
 * handle_builtins - handle builtin commands (exit, env)
 * @tokens: tokenized command
 * @status: pointer to last status
 *
 * Return: 1 if shell should exit, 0 otherwise
 */
int handle_builtins(char **tokens, int *status)
{
	if (!tokens || !tokens[0])
		return (0);

	if (strcmp(tokens[0], "exit") == 0)
		return (1);

	if (strcmp(tokens[0], "env") == 0)
	{
		print_env();
		*status = 0;
		return (0);
	}

	return (0);
}

/**
 * print_env - print current environment
 */
static void print_env(void)
{
	int i = 0;
	size_t len;

	while (environ[i])
	{
		len = strlen(environ[i]);
		write(STDOUT_FILENO, environ[i], len);
		write(STDOUT_FILENO, "\n", 1);
		i++;
	}
}
