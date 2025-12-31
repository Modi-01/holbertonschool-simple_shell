#include "hsh.h"

static char **g_env_heap;          /* heap environ array if we created one */
static char **g_allocs;            /* pointers to strings we malloc'ed */
static size_t g_allocs_len;

static size_t env_count(void);
static int env_index_of(const char *name);
static int env_name_match(const char *envstr, const char *name);
static int name_is_valid(const char *name);
static char *make_env_entry(const char *name, const char *value);

static int add_alloc(char *p);
static void remove_alloc(char *p);
static void free_allocs(void);

static int usage_setenv(int *status);
static int usage_unsetenv(int *status);

/**
 * builtin_setenv - set or modify an environment variable
 * Syntax: setenv VARIABLE VALUE
 * @tokens: argv array
 * @status: pointer to last status
 *
 * Return: 0 always (shell continues)
 */
int builtin_setenv(char **tokens, int *status)
{
	int idx;
	char *entry;
	char **newenv;
	size_t n, i;
	char **oldenv;

	if (!tokens || !tokens[0])
		return (0);

	/* must be exactly 3 tokens: setenv VAR VALUE */
	if (!tokens[1] || !tokens[2] || tokens[3])
		return (usage_setenv(status));

	if (!name_is_valid(tokens[1]))
	{
		fprintf(stderr, "%s: %lu: setenv: invalid variable name\n",
			g_argv0 ? g_argv0 : "./hsh", g_ln);
		if (status)
			*status = 1;
		return (0);
	}

	entry = make_env_entry(tokens[1], tokens[2]);
	if (!entry)
	{
		fprintf(stderr, "%s: %lu: setenv: allocation failed\n",
			g_argv0 ? g_argv0 : "./hsh", g_ln);
		if (status)
			*status = 1;
		return (0);
	}

	if (add_alloc(entry) != 0)
	{
		free(entry);
		fprintf(stderr, "%s: %lu: setenv: allocation failed\n",
			g_argv0 ? g_argv0 : "./hsh", g_ln);
		if (status)
			*status = 1;
		return (0);
	}

	idx = env_index_of(tokens[1]);
	if (idx >= 0)
	{
		/* replace existing */
		remove_alloc(environ[idx]); /* only frees if it was ours */
		environ[idx] = entry;

		if (status)
			*status = 0;
		return (0);
	}

	/* append new variable */
	oldenv = environ;
	n = env_count();

	newenv = malloc(sizeof(char *) * (n + 2));
	if (!newenv)
	{
		remove_alloc(entry);
		free(entry);
		fprintf(stderr, "%s: %lu: setenv: allocation failed\n",
			g_argv0 ? g_argv0 : "./hsh", g_ln);
		if (status)
			*status = 1;
		return (0);
	}

	for (i = 0; i < n; i++)
		newenv[i] = oldenv[i];

	newenv[n] = entry;
	newenv[n + 1] = NULL;

	environ = newenv;

	/* free old environ array only if it was created by us */
	if (g_env_heap && oldenv == g_env_heap)
		free(g_env_heap);

	g_env_heap = newenv;

	if (status)
		*status = 0;
	return (0);
}

/**
 * builtin_unsetenv - remove an environment variable
 * Syntax: unsetenv VARIABLE
 * @tokens: argv array
 * @status: pointer to last status
 *
 * Return: 0 always (shell continues)
 */
int builtin_unsetenv(char **tokens, int *status)
{
	int idx;
	char **newenv;
	size_t n, i, j;
	char **oldenv;

	if (!tokens || !tokens[0])
		return (0);

	/* must be exactly 2 tokens: unsetenv VAR */
	if (!tokens[1] || tokens[2])
		return (usage_unsetenv(status));

	if (!name_is_valid(tokens[1]))
	{
		fprintf(stderr, "%s: %lu: unsetenv: invalid variable name\n",
			g_argv0 ? g_argv0 : "./hsh", g_ln);
		if (status)
			*status = 1;
		return (0);
	}

	idx = env_index_of(tokens[1]);
	if (idx < 0)
	{
		/* Not existing: treat as success (no output) */
		if (status)
			*status = 0;
		return (0);
	}

	oldenv = environ;
	n = env_count();

	newenv = malloc(sizeof(char *) * n); /* n-1 vars + NULL */
	if (!newenv)
	{
		fprintf(stderr, "%s: %lu: unsetenv: allocation failed\n",
			g_argv0 ? g_argv0 : "./hsh", g_ln);
		if (status)
			*status = 1;
		return (0);
	}

	j = 0;
	for (i = 0; i < n; i++)
	{
		if ((int)i == idx)
			continue;
		newenv[j++] = oldenv[i];
	}
	newenv[j] = NULL;

	/* free removed string only if it was ours */
	remove_alloc(oldenv[idx]);

	environ = newenv;

	if (g_env_heap && oldenv == g_env_heap)
		free(g_env_heap);

	g_env_heap = newenv;

	if (status)
		*status = 0;
	return (0);
}

/**
 * env_cleanup - free any heap allocations we did for env management
 */
void env_cleanup(void)
{
	free_allocs();

	if (g_env_heap)
	{
		free(g_env_heap);
		g_env_heap = NULL;
	}
}

static int usage_setenv(int *status)
{
	fprintf(stderr, "%s: %lu: setenv: usage: setenv VARIABLE VALUE\n",
		g_argv0 ? g_argv0 : "./hsh", g_ln);
	if (status)
		*status = 1;
	return (0);
}

static int usage_unsetenv(int *status)
{
	fprintf(stderr, "%s: %lu: unsetenv: usage: unsetenv VARIABLE\n",
		g_argv0 ? g_argv0 : "./hsh", g_ln);
	if (status)
		*status = 1;
	return (0);
}

static size_t env_count(void)
{
	size_t n = 0;

	while (environ && environ[n])
		n++;
	return (n);
}

static int env_name_match(const char *envstr, const char *name)
{
	size_t i = 0;
	size_t len;

	if (!envstr || !name)
		return (0);

	len = _strlen(name);

	while (i < len)
	{
		if (envstr[i] != name[i])
			return (0);
		i++;
	}
	return (envstr[len] == '=');
}

static int env_index_of(const char *name)
{
	size_t i = 0;

	while (environ && environ[i])
	{
		if (env_name_match(environ[i], name))
			return ((int)i);
		i++;
	}
	return (-1);
}

static int name_is_valid(const char *name)
{
	size_t i = 0;

	if (!name || name[0] == '\0')
		return (0);

	while (name[i])
	{
		if (name[i] == '=')
			return (0);
		i++;
	}
	return (1);
}

static char *make_env_entry(const char *name, const char *value)
{
	size_t ln, lv, total;
	char *s;
	size_t i, j;

	ln = _strlen(name);
	lv = _strlen(value);
	total = ln + 1 + lv + 1;

	s = malloc(total);
	if (!s)
		return (NULL);

	for (i = 0; i < ln; i++)
		s[i] = name[i];

	s[i++] = '=';

	for (j = 0; j < lv; j++)
		s[i + j] = value[j];

	s[i + j] = '\0';
	return (s);
}

static int add_alloc(char *p)
{
	char **tmp;
	size_t i;

	if (!p)
		return (-1);

	tmp = malloc(sizeof(char *) * (g_allocs_len + 1));
	if (!tmp)
		return (-1);

	for (i = 0; i < g_allocs_len; i++)
		tmp[i] = g_allocs[i];

	tmp[g_allocs_len] = p;

	free(g_allocs);
	g_allocs = tmp;
	g_allocs_len++;

	return (0);
}

static void remove_alloc(char *p)
{
	size_t i, j;
	char **tmp;

	if (!p || !g_allocs || g_allocs_len == 0)
		return;

	for (i = 0; i < g_allocs_len; i++)
	{
		if (g_allocs[i] == p)
		{
			free(p);

			tmp = NULL;
			if (g_allocs_len > 1)
			{
				tmp = malloc(sizeof(char *) * (g_allocs_len - 1));
				if (!tmp)
					return; /* worst-case: keep list, avoid crash */
			}

			j = 0;
			for (i = 0; i < g_allocs_len; i++)
			{
				if (g_allocs[i] == p)
					continue;
				if (tmp)
					tmp[j] = g_allocs[i];
				j++;
			}

			free(g_allocs);
			g_allocs = tmp;
			g_allocs_len--;
			return;
		}
	}
}

static void free_allocs(void)
{
	size_t i;

	for (i = 0; i < g_allocs_len; i++)
		free(g_allocs[i]);

	free(g_allocs);
	g_allocs = NULL;
	g_allocs_len = 0;
}
