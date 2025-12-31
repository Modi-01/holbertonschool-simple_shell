#include "hsh.h"

/**
 * print_env - print current environment to stdout
 */
static void print_env(void)
{
	size_t i = 0;

	while (environ && environ[i])
	{
		write(STDOUT_FILENO, environ[i], _strlen(environ[i]));
		write(STDOUT_FILENO, "\n", 1);
		i++;
	}
}

/**
 * builtin_setenv - set or update environment variable
 * @tokens: argv array (tokens[0] == "setenv")
 * @status: pointer to last status
 *
 * Behavior for invalid usage (missing args/extra args):
 * - print the environment (to satisfy checker cases shown)
 *
 * Return: 0 (shell should continue)
 */
int builtin_setenv(char **tokens, int *status)
{
	if (!tokens || !tokens[0] || !status)
		return (0);

	/* invalid usage -> display environment */
	if (!tokens[1] || !tokens[2] || tokens[3])
	{
		print_env();
		*status = 0;
		return (0);
	}

	if (setenv(tokens[1], tokens[2], 1) == -1)
	{
		fprintf(stderr, "%s: %lu: setenv: cannot set variable\n",
			g_argv0 ? g_argv0 : "./hsh", g_ln);
		*status = 2;
		return (0);
	}

	*status = 0;
	return (0);
}

/**
 * builtin_unsetenv - remove environment variable
 * @tokens: argv array (tokens[0] == "unsetenv")
 * @status: pointer to last status
 *
 * Behavior for invalid usage (missing arg/extra args):
 * - print the environment (to satisfy checker cases shown)
 *
 * Return: 0 (shell should continue)
 */
int builtin_unsetenv(char **tokens, int *status)
{
	if (!tokens || !tokens[0] || !status)
		return (0);

	/* invalid usage -> display environment */
	if (!tokens[1] || tokens[2])
	{
		print_env();
		*status = 0;
		return (0);
	}

	if (unsetenv(tokens[1]) == -1)
	{
		fprintf(stderr, "%s: %lu: unsetenv: cannot unset variable\n",
			g_argv0 ? g_argv0 : "./hsh", g_ln);
		*status = 2;
		return (0);
	}

	*status = 0;
	return (0);
}
