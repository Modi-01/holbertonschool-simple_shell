#include "hsh.h"
#include <limits.h>

/**
 * is_number - check if string is a valid integer (optional + or -)
 * @s: string
 *
 * Return: 1 if valid, 0 otherwise
 */
static int is_number(char *s)
{
	size_t i = 0;

	if (!s || s[0] == '\0')
		return (0);

	if (s[i] == '+' || s[i] == '-')
		i++;

	if (s[i] == '\0')
		return (0);

	for (; s[i]; i++)
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
	}
	return (1);
}

/**
 * to_long - convert numeric string to long (no locale)
 * @s: string (assumed valid by is_number)
 *
 * Return: long value
 */
static long to_long(char *s)
{
	long sign = 1;
	long n = 0;
	size_t i = 0;

	if (s[i] == '+' || s[i] == '-')
	{
		if (s[i] == '-')
			sign = -1;
		i++;
	}

	for (; s[i]; i++)
	{
		/* basic overflow-safe-ish accumulation */
		if (n > (LONG_MAX - (s[i] - '0')) / 10)
			return (LONG_MAX * sign);
		n = n * 10 + (s[i] - '0');
	}

	return (n * sign);
}

/**
 * print_exit_illegal - print: exit: Illegal number: X
 * @arg: the illegal argument
 */
static void print_exit_illegal(char *arg)
{
	char *p1 = "exit: Illegal number: ";
	char *p2 = "\n";

	write(STDERR_FILENO, p1, _strlen(p1));
	if (arg)
		write(STDERR_FILENO, arg, _strlen(arg));
	write(STDERR_FILENO, p2, 1);
}

/**
 * print_exit_too_many - print: exit: too many arguments
 */
static void print_exit_too_many(void)
{
	char *msg = "exit: too many arguments\n";

	write(STDERR_FILENO, msg, _strlen(msg));
}

/**
 * handle_builtins - handle built-in commands
 * @tokens: tokenized input
 * @status: pointer to last status
 *
 * Return: 1 if should exit shell, 0 otherwise
 */
int handle_builtins(char **tokens, int *status)
{
	long n;

	if (!tokens || !tokens[0])
		return (0);

	if (_strcmp(tokens[0], "exit") == 0)
	{
		/* case: exit (no arg) */
		if (!tokens[1])
			return (1);

		/* case: exit with too many args => don't exit */
		if (tokens[2])
		{
			print_exit_too_many();
			if (status)
				*status = 2;
			return (0);
		}

		/* case: exit with illegal number => exit with 2 */
		if (!is_number(tokens[1]))
		{
			print_exit_illegal(tokens[1]);
			if (status)
				*status = 2;
			return (1);
		}

		/* valid number: exit with N % 256 */
		n = to_long(tokens[1]);
		if (status)
			*status = (unsigned char)n;
		return (1);
	}

	/* باقي الـ builtins عندك (env, cd, ...) */
	return (0);
}
