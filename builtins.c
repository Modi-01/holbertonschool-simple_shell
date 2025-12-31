#include "hsh.h"

static int is_unsigned_number(char *s);
static unsigned long to_ulong(char *s);
static void print_illegal_exit(char *argv0, unsigned long ln, char *arg);

/**
 * handle_builtins - handle built-in commands
 * @tokens: command tokens
 * @status: last status (in/out)
 *
 * Return: 1 if shell should stop, 0 otherwise
 */
int handle_builtins(char **tokens, int *status)
{
	unsigned long code;

	if (!tokens || !tokens[0])
		return (0);

	if (_strcmp(tokens[0], "exit") == 0)
	{
		if (!tokens[1])
			return (1);

		if (!is_unsigned_number(tokens[1]))
		{
			print_illegal_exit(tokens[-1], 0, tokens[1]);
			*status = 2;
			return (0);
		}

		code = to_ulong(tokens[1]);
		*status = (int)(code % 256);
		return (1);
	}

	return (0);
}

static int is_unsigned_number(char *s)
{
	size_t i;

	if (!s || s[0] == '\0')
		return (0);
	if (s[0] == '-')
		return (0);

	i = 0;
	if (s[0] == '+')
		i = 1;

	for (; s[i]; i++)
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
	}
	return (1);
}

static unsigned long to_ulong(char *s)
{
	unsigned long n;
	size_t i;

	n = 0;
	i = 0;
	if (s[0] == '+')
		i = 1;

	for (; s[i]; i++)
		n = n * 10 + (s[i] - '0');

	return (n);
}

static void print_illegal_exit(char *argv0, unsigned long ln, char *arg)
{
	char *p1 = ": ";
	char *p2 = ": exit: Illegal number: ";
	char nl = '\n';

	if (!argv0)
		argv0 = "./hsh";

	write(STDERR_FILENO, argv0, _strlen(argv0));
	write(STDERR_FILENO, p1, 2);

	/* ln printing (minimal): */
	{
		char buf[32];
		int idx = 0;
		unsigned long x = ln;

		if (x == 0)
			buf[idx++] = '1';
		else
		{
			char tmp[32];
			int t = 0;

			while (x > 0)
			{
				tmp[t++] = (x % 10) + '0';
				x /= 10;
			}
			while (t--)
				buf[idx++] = tmp[t];
		}
		write(STDERR_FILENO, buf, idx);
	}

	write(STDERR_FILENO, p2, _strlen(p2));
	write(STDERR_FILENO, arg, _strlen(arg));
	write(STDERR_FILENO, &nl, 1);
}
