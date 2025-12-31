#include "hsh.h"

typedef struct env_alloc_s
{
	void *ptr;
	int type; /* 0 = string, 1 = array */
	struct env_alloc_s *next;
} env_alloc_t;

static env_alloc_t *g_env_allocs = NULL;

static int is_unsigned_number(const char *s);
static unsigned long to_ulong(const char *s);

int builtin_setenv(char **tokens, int *status);
int builtin_unsetenv(char **tokens, int *status);

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
		env_alloc_t *node = g_env_allocs, *tmp;

		/* exit without parameter */
		if (!tokens[1])
		{
			while (node)
			{
				tmp = node->next;
				if (node->ptr)
					free(node->ptr);
				free(node);
				node = tmp;
			}
			g_env_allocs = NULL;
			return (1);
		}

		/* reject negative or non-digit */
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

		while (node)
		{
			tmp = node->next;
			if (node->ptr)
				free(node->ptr);
			free(node);
			node = tmp;
		}
		g_env_allocs = NULL;

		return (1);
	}

	return (0);
}

/**
 * builtin_setenv - set or modify environment variable
 * @tokens: argv array (tokens[1]=VAR, tokens[2]=VALUE)
 * @status: pointer to last status (set to 1 on failure)
 *
 * Return: 0 (shell continues)
 */
int builtin_setenv(char **tokens, int *status)
{
	size_t i = 0, count = 0, var_len, val_len, k;
	char *var, *val, *newstr;
	char **newenv, **oldenv;
	env_alloc_t *node;

	if (!tokens || !tokens[1] || !tokens[2])
	{
		fprintf(stderr, "setenv: usage: setenv VARIABLE VALUE\n");
		if (status)
			*status = 1;
		return (0);
	}

	var = tokens[1];
	val = tokens[2];

	var_len = _strlen(var);
	val_len = _strlen(val);

	if (var_len == 0)
	{
		fprintf(stderr, "setenv: invalid variable name\n");
		if (status)
			*status = 1;
		return (0);
	}

	for (k = 0; k < var_len; k++)
	{
		if (var[k] == '=')
		{
			fprintf(stderr, "setenv: invalid variable name\n");
			if (status)
				*status = 1;
			return (0);
		}
	}

	newstr = malloc(var_len + 1 + val_len + 1);
	if (!newstr)
	{
		fprintf(stderr, "setenv: malloc failed\n");
		if (status)
			*status = 1;
		return (0);
	}

	for (k = 0; k < var_len; k++)
		newstr[k] = var[k];
	newstr[var_len] = '=';
	for (k = 0; k < val_len; k++)
		newstr[var_len + 1 + k] = val[k];
	newstr[var_len + 1 + val_len] = '\0';

	node = malloc(sizeof(*node));
	if (!node)
	{
		free(newstr);
		fprintf(stderr, "setenv: malloc failed\n");
		if (status)
			*status = 1;
		return (0);
	}
	node->ptr = newstr;
	node->type = 0;
	node->next = g_env_allocs;
	g_env_allocs = node;

	while (environ && environ[count])
		count++;

	/* if exists, replace */
	for (i = 0; environ && environ[i]; i++)
	{
		if (_strncmp(environ[i], var, var_len) == 0 &&
		    environ[i][var_len] == '=')
		{
			/* free old string only if we own it */
			node = g_env_allocs;
			while (node)
			{
				if (node->type == 0 && node->ptr == environ[i])
				{
					free(node->ptr);
					node->ptr = NULL;
					break;
				}
				node = node->next;
			}
			environ[i] = newstr;
			return (0);
		}
	}

	/* not found: grow environ */
	newenv = malloc(sizeof(char *) * (count + 2));
	if (!newenv)
	{
		fprintf(stderr, "setenv: malloc failed\n");
		if (status)
			*status = 1;
		return (0);
	}

	for (i = 0; i < count; i++)
		newenv[i] = environ[i];

	newenv[count] = newstr;
	newenv[count + 1] = NULL;

	oldenv = environ;
	environ = newenv;

	/* track new array */
	node = malloc(sizeof(*node));
	if (!node)
	{
		fprintf(stderr, "setenv: malloc failed\n");
		if (status)
			*status = 1;
		return (0);
	}
	node->ptr = newenv;
	node->type = 1;
	node->next = g_env_allocs;
	g_env_allocs = node;

	/* free old array only if we own it */
	node = g_env_allocs;
	while (node)
	{
		if (node->type == 1 && node->ptr == oldenv)
		{
			free(node->ptr);
			node->ptr = NULL;
			break;
		}
		node = node->next;
	}

	return (0);
}

/**
 * builtin_unsetenv - remove environment variable
 * @tokens: argv array (tokens[1]=VAR)
 * @status: pointer to last status (set to 1 on failure)
 *
 * Return: 0 (shell continues)
 */
int builtin_unsetenv(char **tokens, int *status)
{
	size_t i = 0, j = 0, count = 0, var_len;
	char *var;
	char **newenv, **oldenv;
	env_alloc_t *node;

	if (!tokens || !tokens[1])
	{
		fprintf(stderr, "unsetenv: usage: unsetenv VARIABLE\n");
		if (status)
			*status = 1;
		return (0);
	}

	var = tokens[1];
	var_len = _strlen(var);

	if (var_len == 0)
	{
		fprintf(stderr, "unsetenv: invalid variable name\n");
		if (status)
			*status = 1;
		return (0);
	}

	while (environ && environ[count])
		count++;

	newenv = malloc(sizeof(char *) * (count + 1));
	if (!newenv)
	{
		fprintf(stderr, "unsetenv: malloc failed\n");
		if (status)
			*status = 1;
		return (0);
	}

	for (i = 0; environ && environ[i]; i++)
	{
		if (_strncmp(environ[i], var, var_len) == 0 &&
		    environ[i][var_len] == '=')
		{
			/* free removed string only if we own it */
			node = g_env_allocs;
			while (node)
			{
				if (node->type == 0 && node->ptr == environ[i])
				{
					free(node->ptr);
					node->ptr = NULL;
					break;
				}
				node = node->next;
			}
			continue;
		}
		newenv[j++] = environ[i];
	}
	newenv[j] = NULL;

	oldenv = environ;
	environ = newenv;

	/* track new array */
	node = malloc(sizeof(*node));
	if (!node)
	{
		fprintf(stderr, "unsetenv: malloc failed\n");
		if (status)
			*status = 1;
		return (0);
	}
	node->ptr = newenv;
	node->type = 1;
	node->next = g_env_allocs;
	g_env_allocs = node;

	/* free old array only if we own it */
	node = g_env_allocs;
	while (node)
	{
		if (node->type == 1 && node->ptr == oldenv)
		{
			free(node->ptr);
			node->ptr = NULL;
			break;
		}
		node = node->next;
	}

	return (0);
}

/**
 * is_unsigned_number - check if string is digits only
 * @s: string
 *
 * Return: 1 if digits only, 0 otherwise
 */
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

/**
 * to_ulong - convert digits string to unsigned long
 * @s: string
 *
 * Return: number
 */
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
