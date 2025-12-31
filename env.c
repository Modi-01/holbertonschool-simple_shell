#include "hsh.h"

/* exported so cleanup.c can free them */
int g_env_cloned = 0;
char **g_env_alloc = NULL;

static int env_name_ok(const char *name, int *len);
static int env_find(const char *name, int len);
static int env_clone(void);

/**
 * shell_setenv - set or overwrite env var
 * @name: variable name
 * @value: variable value
 * Return: 0 on success, -1 on failure
 */
int shell_setenv(const char *name, const char *value)
{
	int len, idx;
	size_t vlen, total, i, count;
	char *entry;
	char **newenv;

	if (env_clone() == -1)
		return (-1);

	if (!env_name_ok(name, &len))
		return (-1);

	if (!value)
		value = "";

	idx = env_find(name, len);

	vlen = _strlen(value);
	total = (size_t)len + 1 + vlen + 1;

	entry = malloc(total);
	if (!entry)
		return (-1);

	for (i = 0; i < (size_t)len; i++)
		entry[i] = name[i];
	entry[i++] = '=';
	for (count = 0; count < vlen; count++)
		entry[i + count] = value[count];
	entry[i + vlen] = '\0';

	if (idx >= 0)
	{
		free(environ[idx]);
		environ[idx] = entry;
		return (0);
	}

	/* append (no realloc allowed) */
	count = 0;
	while (environ[count])
		count++;

	newenv = malloc(sizeof(char *) * (count + 2));
	if (!newenv)
	{
		free(entry);
		return (-1);
	}

	for (i = 0; i < count; i++)
		newenv[i] = environ[i];

	newenv[count] = entry;
	newenv[count + 1] = NULL;

	free(environ);
	environ = newenv;
	g_env_alloc = newenv;

	return (0);
}

/**
 * shell_unsetenv - remove env var
 * @name: variable name
 * Return: 0 on success, -1 on failure
 */
int shell_unsetenv(const char *name)
{
	int len, idx;
	size_t i;

	if (env_clone() == -1)
		return (-1);

	if (!env_name_ok(name, &len))
		return (-1);

	idx = env_find(name, len);
	if (idx < 0)
		return (0);

	free(environ[idx]);

	i = (size_t)idx;
	while (environ[i + 1])
	{
		environ[i] = environ[i + 1];
		i++;
	}
	environ[i] = NULL;

	return (0);
}

static int env_clone(void)
{
	size_t count = 0, i, len;
	char **newenv;
	char *dup;

	if (g_env_cloned)
		return (0);

	while (environ && environ[count])
		count++;

	newenv = malloc(sizeof(char *) * (count + 1));
	if (!newenv)
		return (-1);

	for (i = 0; i < count; i++)
	{
		len = _strlen(environ[i]);
		dup = malloc(len + 1);
		if (!dup)
		{
			while (i > 0)
				free(newenv[--i]);
			free(newenv);
			return (-1);
		}
		_strcpy(dup, environ[i]);
		newenv[i] = dup;
	}
	newenv[count] = NULL;

	environ = newenv;
	g_env_alloc = newenv;
	g_env_cloned = 1;

	return (0);
}

static int env_find(const char *name, int len)
{
	int i;

	for (i = 0; environ && environ[i]; i++)
	{
		if (_strncmp(environ[i], name, (size_t)len) == 0 &&
		    environ[i][len] == '=')
			return (i);
	}
	return (-1);
}

static int env_name_ok(const char *name, int *len)
{
	int i = 0;

	if (!name || name[0] == '\0')
		return (0);

	while (name[i])
	{
		if (name[i] == '=')
			return (0);
		i++;
	}
	*len = i;
	return (1);
}
