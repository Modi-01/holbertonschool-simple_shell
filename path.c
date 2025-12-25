#include "hsh.h"

static char *_getenv(char *name);

/**
 * find_in_path - search for command in PATH
 * @cmd: command name
 *
 * Return: malloc'd full path if found, NULL otherwise
 */
char *find_in_path(char *cmd)
{
	char *path, *copy, *dir;
	char *full;
	size_t len;

	if (!cmd || strchr(cmd, '/'))
		return (NULL);

	path = _getenv("PATH");
	if (!path)
		return (NULL);

	copy = malloc(strlen(path) + 1);
	if (!copy)
		return (NULL);
	strcpy(copy, path);

	dir = strtok(copy, ":");
	while (dir)
	{
		len = strlen(dir) + 1 + strlen(cmd) + 1;
		full = malloc(len);
		if (!full)
		{
			free(copy);
			return (NULL);
		}

		strcpy(full, dir);
		strcat(full, "/");
		strcat(full, cmd);

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
 * _getenv - get environment variable value without using getenv
 * @name: env key
 *
 * Return: pointer to value inside environ, or NULL
 */
static char *_getenv(char *name)
{
	size_t i, len;

	if (!name)
		return (NULL);

	len = strlen(name);
	for (i = 0; environ[i]; i++)
	{
		if (strncmp(environ[i], name, len) == 0 && environ[i][len] == '=')
			return (environ[i] + len + 1);
	}
	return (NULL);
}
