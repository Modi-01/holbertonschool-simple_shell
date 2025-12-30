#include "hsh.h"

/**
 * handle_builtins - handle builtin commands
 * @tokens: tokenized command
 *
 * Return: 1 if shell should exit, 0 otherwise
 */
int handle_builtins(char **tokens)
{
	if (!tokens || !tokens[0])
		return (0);

	if (strcmp(tokens[0], "exit") == 0)
		return (1);

	return (0);
}
