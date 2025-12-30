#include "hsh.h"

static int interactive_mode(void);
static void print_prompt(void);
static void strip_newline(char *s);
static int is_blank(char *s);

/**
 * run_shell - simple shell 1.0 (PATH + args + exit/env) using custom _getline
 * @argv0: program name
 *
 * Return: last status
 */
int run_shell(char *argv0)
{
	char *line = NULL;
	size_t n = 0;
	ssize_t r;
	unsigned long ln = 0;
	int status = 0;
	int interactive = interactive_mode();
	char **tokens = NULL;

	while (1)
	{
		if (interactive)
			print_prompt();

		r = _getline(&line, &n, STDIN_FILENO);
		if (r == -1)
		{
			if (interactive)
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		ln++;
		strip_newline(line);

		if (is_blank(line))
			continue;

		tokens = split_line(line);
		if (!tokens || !tokens[0])
		{
			free_tokens(tokens);
			continue;
		}

		if (handle_builtins(tokens, &status))
		{
			free_tokens(tokens);
			break;
		}

		execute_tokens(tokens, argv0, ln, &status);
		free_tokens(tokens);
	}

	free(line);
	return (status);
}

/**
 * interactive_mode - check if stdin is a terminal
 *
 * Return: 1 if interactive, 0 otherwise
 */
static int interactive_mode(void)
{
	return (isatty(STDIN_FILENO));
}

/**
 * print_prompt - display prompt
 */
static void print_prompt(void)
{
	write(STDOUT_FILENO, "($) ", 4);
}

/**
 * strip_newline - remove trailing newline
 * @s: string
 */
static void strip_newline(char *s)
{
	size_t i = 0;

	if (!s)
		return;

	while (s[i] != '\0')
		i++;

	if (i > 0 && s[i - 1] == '\n')
		s[i - 1] = '\0';
}

/**
 * is_blank - check if line contains only spaces/tabs or is empty
 * @s: string
 *
 * Return: 1 if blank, 0 otherwise
 */
static int is_blank(char *s)
{
	size_t i = 0;

	if (!s || s[0] == '\0')
		return (1);

	while (s[i])
	{
		if (s[i] != ' ' && s[i] != '\t')
			return (0);
		i++;
	}
	return (1);
}
