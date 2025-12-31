#include "hsh.h"

static char *get_env_value(const char *name)
{
	int i;
	size_t nlen;

	if (!name || !environ)
		return (NULL);

	nlen = _strlen(name);

	for (i = 0; environ[i]; i++)
	{
		if (_strncmp(environ[i], name, nlen) == 0 &&
		    environ[i][nlen] == '=')
			return (environ[i] + nlen + 1);
	}
	return (NULL);
}

static char *getcwd_alloc(void)
{
	char *buf;
	size_t size;

	size = 128;
	while (1)
	{
		buf = malloc(size);
		if (!buf)
			return (NULL);

		if (getcwd(buf, size) != NULL)
			return (buf);

		free(buf);
		if (errno != ERANGE)
			return (NULL);

		size *= 2;
	}
}

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

static char *build_cd_errmsg(const char *dir)
{
	size_t len1, len2;
	char *msg;

	if (!dir)
		dir = "";

	len1 = _strlen("can't cd to ");
	len2 = _strlen(dir);

	msg = malloc(len1 + len2 + 2);
	if (!msg)
		return (NULL);

	_strcpy(msg, "can't cd to ");
	_strcpy(msg + len1, dir);
	msg[len1 + len2] = '\n';
	msg[len1 + len2 + 1] = '\0';

	return (msg);
}

/**
 * builtin_cd - change current directory
 * @tokens: tokenized input
 * @status: pointer to last status
 *
 * Return: 0 always (shell continues)
 */
int builtin_cd(char **tokens, int *status)
{
	char *oldpwd, *newpwd, *target;
	int print_newpwd;

	print_newpwd = 0;
	oldpwd = getcwd_alloc();
	if (!oldpwd)
	{
		print_error(g_argv0, g_ln, "cd", "failed\n");
		*status = 2;
		return (0);
	}

	if (!tokens[1])
	{
		target = get_env_value("HOME");
		if (!target)
		{
			free(oldpwd);
			print_error(g_argv0, g_ln, "cd", "HOME not set\n");
			*status = 2;
			return (0);
		}
	}
	else if (_strcmp(tokens[1], "-") == 0)
	{
		target = get_env_value("OLDPWD");
		if (!target)
		{
			free(oldpwd);
			print_error(g_argv0, g_ln, "cd", "OLDPWD not set\n");
			*status = 2;
			return (0);
		}
		print_newpwd = 1;
	}
	else
	{
		if (tokens[2])
		{
			free(oldpwd);
			print_error(g_argv0, g_ln, "cd", "too many arguments\n");
			*status = 2;
			return (0);
		}
		target = tokens[1];
	}

	if (chdir(target) == -1)
	{
		char *msg;

		msg = build_cd_errmsg(target);
		if (msg)
		{
			print_error(g_argv0, g_ln, "cd", msg);
			free(msg);
		}
		else
			print_error(g_argv0, g_ln, "cd", "failed\n");

		free(oldpwd);
		*status = 2;
		return (0);
	}

	newpwd = getcwd_alloc();
	if (!newpwd)
	{
		free(oldpwd);
		print_error(g_argv0, g_ln, "cd", "failed\n");
		*status = 2;
		return (0);
	}

	(void)shell_setenv("OLDPWD", oldpwd);
	(void)shell_setenv("PWD", newpwd);

	if (print_newpwd)
	{
		write(STDOUT_FILENO, newpwd, _strlen(newpwd));
		write(STDOUT_FILENO, "\n", 1);
	}

	free(oldpwd);
	free(newpwd);
	*status = 0;
	return (0);
}

/**
 * handle_builtins - handle shell builtins
 * @tokens: tokenized input
 * @status: pointer to last command status
 *
 * Return: 1 if should exit shell, 0 otherwise
 */
int handle_builtins(char **tokens, int *status)
{
	int i;
	char *env_line;

	if (!tokens || !tokens[0])
		return (0);

	if (_strcmp(tokens[0], "env") == 0)
	{
		print_env_stdout();
		*status = 0;
		return (0);
	}

	if (_strcmp(tokens[0], "setenv") == 0)
		return (builtin_setenv(tokens, status));

	if (_strcmp(tokens[0], "unsetenv") == 0)
		return (builtin_unsetenv(tokens, status));

	if (_strcmp(tokens[0], "cd") == 0)
		return (builtin_cd(tokens, status));

	if (_strcmp(tokens[0], "exit") == 0)
	{
		if (!tokens[1])
			return (1);

		for (i = 0; tokens[1][i]; i++)
		{
			if (tokens[1][i] < '0' || tokens[1][i] > '9')
			{
				print_error(g_argv0, g_ln, "exit",
					"Illegal number\n");
				*status = 2;
				return (0);
			}
		}
		*status = atoi(tokens[1]);
		return (1);
	}

	(void)env_line;
	return (0);
}
