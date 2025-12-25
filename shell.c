#include "hsh.h"

static int interactive_mode(void);
static int is_number(char *s);
static int str_to_int(char *s);
static int handle_builtin(char **tokens, int *status, char *argv0,
		unsigned long ln);

/**
 * run_shell - main shell loop
 * @argv0: program name (av[0])
 *
 * Return: exit status
 */
int run_shell(char *argv0)
{
	char *line = NULL;
	size_t n = 0;
	ssize_t r;
	char **tokens = NULL;
	unsigned long ln = 0;
	int status = 0;
	int interactive = interactive_mode();

	while (1)
	{
		if (interactive)
			write(STDOUT_FILENO, "($) ", 4);

		r = getline(&line, &n, stdin);
		if (r == -1)
		{
			if (interactive)
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		ln++;
		if (r > 0 && line[r - 1] == '\n')
			line[r - 1] = '\0';

		tokens = split_line(line);
		if (!tokens || !tokens[0])
		{
			free_tokens(tokens);
			continue;
		}

		if (handle_builtin(tokens, &status, argv0, ln))
		{
			free_tokens(tokens);
			break;
		}

		execute_cmd(tokens, ln, argv0, &status);
		free_tokens(tokens);
	}

	free(line);
	return (status);
}

/**
 * interactive_mode - check if shell is running in interactive mode
 *
 * Return: 1 if interactive, 0 otherwise
 */
static int interactive_mode(void)
{
	return (isatty(STDIN_FILENO));
}

/**
 * handle_builtin - handle built-in commands
 * @tokens: tokenized command
 * @status: pointer to last status
 * @argv0: program name
 * @ln: line number
 *
 * Return: 1 if should exit shell, 0 otherwise
 */
static int handle_builtin(char **tokens, int *status, char *argv0,
		unsigned long ln)
{
	int code;

	if (strcmp(tokens[0], "exit") != 0)
		return (0);

	if (!tokens[1])
		return (1);

	if (!is_number(tokens[1]))
	{
		print_error(argv0, ln, "exit", "Illegal number");
		*status = 2;
		return (0);
	}

	code = str_to_int(tokens[1]);
	*status = code % 256;
	return (1);
}

/**
 * is_number - check if string is a valid number (optional leading +)
 * @s: string
 *
 * Return: 1 if number, 0 otherwise
 */
static int is_number(char *s)
{
	int i = 0;

	if (!s || !s[0])
		return (0);

	if (s[0] == '+')
		i++;

	for (; s[i]; i++)
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
	}
	return (1);
}

/**
 * str_to_int - convert numeric string to int (non-negative)
 * @s: numeric string
 *
 * Return: integer value
 */
static int str_to_int(char *s)
{
	int i = 0;
	int res = 0;

	if (s[0] == '+')
		i++;

	for (; s[i]; i++)
		res = (res * 10) + (s[i] - '0');

	return (res);
}
