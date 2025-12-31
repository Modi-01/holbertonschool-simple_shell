#include "hsh.h"

int g_env_is_ours = 0;

static char *dup_str(const char *s)
{
	size_t len;
	char *p;

	if (!s)
		return (NULL);

	len = _strlen(s);
	p = malloc(len + 1);
	if (!p)
		return (NULL);

	_strcpy(p, s);
	return (p);
}

static int ensure_env_copy(void)
{
	int i, count;
	char **newenv;

	if (g_env_is_ours)
		return (0);

	count = 0;
	while (environ && environ[count])
		count++;

	newenv = malloc(sizeof(char *) * (count + 1));
	if (!newenv)
		return (-1);

	for (i = 0; i < count; i++)
	{
		newenv[i] = dup_str(environ[i]);
		if (!newenv[i])
		{
			while (i > 0)
				free(newenv[--i]);
			free(newenv);
			return (-1);
		}
	}
	newenv[count] = NULL;

	environ = newenv;
	g_env_is_ours = 1;
	return (0);
}

static int find_env_index(const char *name)
{
	int i;
	size_t nlen;

	if (!name || !environ)
		return (-1);

	nlen = _strlen(name);
	for (i = 0; environ[i]; i++)
	{
		if (_strncmp(environ[i], name, nlen) == 0 && environ[i][nlen] == '=')
			return (i);
	}
	return (-1);
}

static char *build_kv(const char *name, const char *value)
{
	size_t nlen, vlen;
	char *kv;

	nlen = _strlen(name);
	vlen = _strlen(value);

	kv = malloc(nlen + vlen + 2);
	if (!kv)
		return (NULL);

	_strcpy(kv, name);
	kv[nlen] = '=';
	_strcpy(kv + nlen + 1, value);
	return (kv);
}

int shell_setenv(const char *name, const char *value)
{
	int idx, i, count;
	char *kv;
	char **newenv;

	if (!name || !value || name[0] == '\0' || _strchr(name, '='))
		return (-1);

	if (ensure_env_copy() == -1)
		return (-1);

	kv = build_kv(name, value);
	if (!kv)
		return (-1);

	idx = find_env_index(name);
	if (idx >= 0)
	{
		free(environ[idx]);
		environ[idx] = kv;
		return (0);
	}

	count = 0;
	while (environ[count])
		count++;

	newenv = malloc(sizeof(char *) * (count + 2));
	if (!newenv)
	{
		free(kv);
		return (-1);
	}

	for (i = 0; i < count; i++)
		newenv[i] = environ[i];

	newenv[count] = kv;
	newenv[count + 1] = NULL;

	free(environ);
	environ = newenv;

	return (0);
}

int shell_unsetenv(const char *name)
{
	int idx, i;

	if (!name || name[0] == '\0' || _strchr(name, '='))
		return (-1);

	if (ensure_env_copy() == -1)
		return (-1);

	idx = find_env_index(name);
	if (idx < 0)
		return (0);

	free(environ[idx]);

	for (i = idx; environ[i]; i++)
		environ[i] = environ[i + 1];

	return (0);
}
