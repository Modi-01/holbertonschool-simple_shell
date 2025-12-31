#include "hsh.h"

static int is_number(const char *s);
static unsigned long to_ulong(const char *s);

/**
 * handle_builtins - handle shell builtins
 * @tokens: argv-like tokens
 * @argv0: program name
 * @ln: line number
 * @status: pointer to last status
 *
 * Return: 1 if shell should terminate, 0 otherwise
 */
int handle_builtins(char **tokens, char *argv0, unsigned long ln, int *status)
{
	unsigned long code;

	if (!tokens || !tokens[0])
		return (0);

	if (_strcmp(tokens[0], "exit") == 0)
	{
		if (!tokens[1])
			exit(*status);

		/* negative or non-numeric => Illegal number, exit(2) */
		if (tokens[1][0] == '-' || !is_number(tokens[1]))
		{
			print_error(argv0, ln, "exit", "Illegal number: ");
			write(STDERR_FILENO, tokens[1], _strlen(tokens[1]));
			write(STDERR_FILENO, "\n", 1);
			exit(2);
		}

		code = to_ulong(tokens[1]);
		exit((int)(code % 256));
	}

	/* add other builtins here (env, cd, etc) */
	return (0);
}

static int is_number(const char *s)
{
	size_t i;

	if (!s || s[0] == '\0')
		return (0);

	i = 0;
	if (s[i] == '+')
		i++;

	if (s[i] == '\0')
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
	unsigned long n;

	n = 0;
	while (*s == '+')
		s++;

	while (*s)
	{
		n = n * 10 + (unsigned long)(*s - '0');
		s++;
	}
	return (n);
}
