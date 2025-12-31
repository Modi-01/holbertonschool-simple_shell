#include "hsh.h"

/**
 * handle_builtins - handle shell builtins
 * @tokens: tokenized input
 * @status: pointer to last command status
 *
 * Return: 1 if should exit shell, 0 otherwise
 */
int handle_builtins(char **tokens, int *status)
{
	int i;
	char *env_line;

	if (!tokens || !tokens[0])
		return (0);

	/* env */
	if (_strcmp(tokens[0], "env") == 0)
	{
		i = 0;
		while (environ && environ[i])
		{
			env_line = environ[i];
			write(STDOUT_FILENO, env_line, _strlen(env_line));
			write(STDOUT_FILENO, "\n", 1);
			i++;
		}
		*status = 0;
		return (0);
	}

	/* setenv / unsetenv (implemented ONLY in env_builtins.c) */
	if (_strcmp(tokens[0], "setenv") == 0)
		return (builtin_setenv(tokens, status));

	if (_strcmp(tokens[0], "unsetenv") == 0)
		return (builtin_unsetenv(tokens, status));

	/* exit */
	if (_strcmp(tokens[0], "exit") == 0)
	{
		if (!tokens[1])
			return (1);

		for (i = 0; tokens[1][i]; i++)
		{
			if (tokens[1][i] < '0' || tokens[1][i] > '9')
			{
				print_error(g_argv0, g_ln, "exit",
					"Illegal number\n");
				*status = 2;
				return (0);
			}
		}
		*status = atoi(tokens[1]);
		return (1);
	}

	return (0);
}
