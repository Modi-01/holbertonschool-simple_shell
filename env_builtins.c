#include "hsh.h"

static void print_env_stdout(void)
{
	int i;
	char *line;

	i = 0;
	while (environ && environ[i])
	{
		line = environ[i];
		write(STDOUT_FILENO, line, _strlen(line));
		write(STDOUT_FILENO, "\n", 1);
		i++;
	}
}

/**
 * builtin_setenv - setenv builtin
 * قواعد الاختبارات:
 * - setenv (بدون باراميتر): يطبع env
 * - setenv NAME (باراميتر واحد): خطأ على stderr فقط (لا يطبع env)
 * - setenv NAME VALUE: ينفذ
 */
int builtin_setenv(char **tokens, int *status)
{
	/* setenv -> print env */
	if (!tokens[1])
	{
		print_env_stdout();
		*status = 0;
		return (0);
	}

	/* setenv NAME -> usage error (no stdout) */
	if (!tokens[2] || tokens[3])
	{
		print_error(g_argv0, g_ln, "setenv",
			"usage: setenv VARIABLE VALUE\n");
		*status = 2;
		return (0);
	}

	if (shell_setenv(tokens[1], tokens[2]) == -1)
	{
		print_error(g_argv0, g_ln, "setenv", "failed\n");
		*status = 2;
		return (0);
	}

	*status = 0;
	return (0);
}

/**
 * builtin_unsetenv - unsetenv builtin
 * قواعد الاختبارات:
 * - unsetenv (بدون باراميتر): يطبع env
 * - unsetenv NAME: ينفذ
 * - unsetenv NAME EXTRA: usage error
 */
int builtin_unsetenv(char **tokens, int *status)
{
	/* unsetenv -> print env */
	if (!tokens[1])
	{
		print_env_stdout();
		*status = 0;
		return (0);
	}

	/* unsetenv NAME EXTRA -> usage error */
	if (tokens[2])
	{
		print_error(g_argv0, g_ln, "unsetenv",
			"usage: unsetenv VARIABLE\n");
		*status = 2;
		return (0);
	}

	/* if variable doesn't exist, treat as success */
	if (shell_unsetenv(tokens[1]) == -1)
	{
		print_error(g_argv0, g_ln, "unsetenv", "failed\n");
		*status = 2;
		return (0);
	}

	*status = 0;
	return (0);
}
