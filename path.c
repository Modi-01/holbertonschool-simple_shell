#include "hsh.h"

static char *_getenv(char *name);

/**
 * resolve_path - resolve command using PATH (cmd without '/')
 * @cmd: command name (e.g., "ls")
 *
 * Return: malloc'd full path if found, NULL otherwise
 */
char *resolve_path(char *cmd)
{
	char *path, *copy, *dir, *full;
	size_t len;

	if (!cmd || _strchr(cmd, '/'))
		return (NULL);

	path = _getenv("PATH");
	if (!path)
		return (NULL);

	copy = malloc(_strlen(path) + 1);
	if (!copy)
		return (NULL);
	_strcpy(copy, path);

	dir = strtok(copy, ":");
	while (dir)
	{
		len = _strlen(dir) + 1 + _strlen(cmd) + 1;
		full = malloc(len);
		if (!full)
		{
			free(copy);
			return (NULL);
		}

		_strcpy(full, dir);
		_strcat(full, "/");
		_strcat(full, cmd);

		if (access(full, X_OK) == 0)
		{
			free(copy);
			return (full);
		}

		free(full);
		dir = strtok(NULL, ":");
	}

	free(copy);
	return (NULL);
}

/**
 * _getenv - get environment variable value from environ
 * @name: env key
 *
 * Return: pointer to value inside environ or NULL
 */
static char *_getenv(char *name)
{
	size_t i, len;

	if (!name)
		return (NULL);

	len = _strlen(name);
	for (i = 0; environ[i]; i++)
	{
		if (strncmp(environ[i], name, len) == 0 && environ[i][len] == '=')
			return (environ[i] + len + 1);
	}
	return (NULL);
}
