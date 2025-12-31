#include "hsh.h"

static int is_unsigned_number(const char *s);
static unsigned long to_ulong(const char *s);

/**
 * handle_builtins - handle built-in commands
 * @tokens: argv array
 * @status: pointer to last status
 *
 * Return: 1 if shell should exit, 0 otherwise
 */
int handle_builtins(char **tokens, int *status)
{
	unsigned long code, mod;

	if (!tokens || !tokens[0])
		return (0);

	if (_strcmp(tokens[0], "env") == 0)
	{
		size_t i = 0;

		while (environ && environ[i])
		{
			write(STDOUT_FILENO, environ[i], _strlen(environ[i]));
			write(STDOUT_FILENO, "\n", 1);
			i++;
		}
		return (0);
	}

	if (_strcmp(tokens[0], "setenv") == 0)
		return (builtin_setenv(tokens, status));

	if (_strcmp(tokens[0], "unsetenv") == 0)
		return (builtin_unsetenv(tokens, status));

	if (_strcmp(tokens[0], "exit") == 0)
	{
		/* exit without parameter */
		if (!tokens[1])
			return (1);

		/* refuse negative or non-numeric */
		if (tokens[1][0] == '-' || !is_unsigned_number(tokens[1]))
		{
			fprintf(stderr, "%s: %lu: exit: Illegal number: %s\n",
				g_argv0 ? g_argv0 : "./hsh", g_ln, tokens[1]);
			*status = 2;
			return (1);
		}

		code = to_ulong(tokens[1]);
		mod = code % 256;
		*status = (int)mod;
		return (1);
	}

	return (0);
}

static int is_unsigned_number(const char *s)
{
	size_t i = 0;

	if (!s || s[0] == '\0')
		return (0);

	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static unsigned long to_ulong(const char *s)
{
	unsigned long n = 0;
	size_t i = 0;

	while (s && s[i])
	{
		n = n * 10 + (unsigned long)(s[i] - '0');
		i++;
	}
	return (n);
}
