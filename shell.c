#include "hsh.h"

char *g_argv0 = NULL;
unsigned long g_ln = 0;

static volatile sig_atomic_t g_interactive = 0;

static void sigint_handler(int sig);
static void print_prompt(void);
static void strip_newline(char *s);
static int is_blank(char *s);

/**
 * run_shell - simple shell (PATH + args + exit/env + Ctrl+C)
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
	char **tokens = NULL;

	g_argv0 = argv0;
	g_interactive = isatty(STDIN_FILENO);

	/* Handle Ctrl+C */
	signal(SIGINT, sigint_handler);

	while (1)
	{
		if (g_interactive)
			print_prompt();

		r = _getline(&line, &n, STDIN_FILENO);

		/* Ctrl+C may interrupt read/getline */
		if (r == -1 && errno == EINTR)
		{
			errno = 0;
			continue;
		}

		if (r == -1)
		{
			if (g_interactive)
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		ln++;
		g_ln = ln;

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

	/* IMPORTANT: correct function name (declared in hsh.h) */
	

	free(line);
	cleanup_env();
	return (status);
}

static void sigint_handler(int sig)
{
	(void)sig;

	/* Must not use printf here (not async-signal-safe) */
	if (g_interactive)
		write(STDOUT_FILENO, "\n($) ", 5);
	else
		write(STDOUT_FILENO, "\n", 1);
}

static void print_prompt(void)
{
	write(STDOUT_FILENO, "($) ", 4);
}

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
