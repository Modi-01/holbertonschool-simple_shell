#include "hsh.h"

static int count_args(char **tokens);

/**
 * builtin_setenv - handle setenv VARIABLE VALUE
 * @tokens: argv
 * @status: status pointer
 * Return: 0 to continue shell
 */
int builtin_setenv(char **tokens, int *status)
{
	int argc;

	argc = count_args(tokens);
	if (argc != 3)
	{
		print_error(g_argv0, g_ln, "setenv", "Usage: setenv VARIABLE VALUE");
		*status = 2;
		return (0);
	}

	if (shell_setenv(tokens[1], tokens[2]) == -1)
	{
		print_error(g_argv0, g_ln, "setenv", "failed");
		*status = 1;
		return (0);
	}

	*status = 0;
	return (0);
}

/**
 * builtin_unsetenv - handle unsetenv VARIABLE
 * @tokens: argv
 * @status: status pointer
 * Return: 0 to continue shell
 */
int builtin_unsetenv(char **tokens, int *status)
{
	int argc;

	argc = count_args(tokens);
	if (argc != 2)
	{
		print_error(g_argv0, g_ln, "unsetenv", "Usage: unsetenv VARIABLE");
		*status = 2;
		return (0);
	}

	if (shell_unsetenv(tokens[1]) == -1)
	{
		print_error(g_argv0, g_ln, "unsetenv", "failed");
		*status = 1;
		return (0);
	}

	*status = 0;
	return (0);
}

static int count_args(char **tokens)
{
	int i = 0;

	if (!tokens)
		return (0);

	while (tokens[i])
		i++;

	return (i);
}
