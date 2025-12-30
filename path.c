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
	size_t len, i, start;
	char save;

	if (!cmd || _strchr(cmd, '/'))
		return (NULL);

	path = _getenv("PATH");
	if (!path)
		return (NULL);

	copy = malloc(_strlen(path) + 1);
	if (!copy)
		return (NULL);

	_strcpy(copy, path);

	start = 0;
	i = 0;
	while (1)
	{
		if (copy[i] == ':' || copy[i] == '\0')
		{
			save = copy[i];
			copy[i] = '\0';

			dir = &copy[start];
			if (dir[0] == '\0')
				dir = ".";

			len = _strlen(dir) + 1 + _strlen(cmd) + 1;
			full = malloc(len);
			if (!full)
			{
				free(copy);
				return (NULL);
			}

			full[0] = '\0';
			_strcpy(full, dir);
			_strcat(full, "/");
			_strcat(full, cmd);

			if (access(full, X_OK) == 0)
			{
				free(copy);
				return (full);
			}

			free(full);
			copy[i] = save;

			if (save == '\0')
				break;

			start = i + 1;
		}
		i++;
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
		if (_strncmp(environ[i], name, len) == 0 && environ[i][len] == '=')
			return (environ[i] + len + 1);
	}
	return (NULL);
}
