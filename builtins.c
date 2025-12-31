#include "hsh.h"

static char **g_env_orig = NULL;
static size_t g_env_len = 0;
static int g_env_owned = 0;

static int is_unsigned_number(const char *s);
static unsigned long to_ulong(const char *s);
static int builtin_setenv(char **tokens);
static int builtin_unsetenv(char **tokens);

/**
 * handle_builtins - handle built-in commands
 * @tokens: argv array
 * @status: pointer to last status
 *
 * Return: 1 if shell should exit, 0 otherwise
 */
int handle_builtins(char **tokens, int *status)
{
	size_t i = 0;

	if (!tokens || !tokens[0])
		return (0);

	if (_strcmp(tokens[0], "env") == 0)
	{
		while (environ && environ[i])
		{
			write(STDOUT_FILENO, environ[i], _strlen(environ[i]));
			write(STDOUT_FILENO, "\n", 1);
			i++;
		}
		return (0);
	}

	if (_strcmp(tokens[0], "setenv") == 0)
		return (builtin_setenv(tokens));

	if (_strcmp(tokens[0], "unsetenv") == 0)
		return (builtin_unsetenv(tokens));

	if (_strcmp(tokens[0], "exit") == 0)
	{
		unsigned long code, mod;

		/* exit بدون باراميتر */
		if (!tokens[1])
			return (1);

		/* رفض السالب أو أي نص غير رقم */
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

		/* قبل الخروج: حرّر نسخة البيئة إذا صرنا نملكها */
		if (g_env_owned)
		{
			i = 0;
			while (environ && environ[i])
			{
				free(environ[i]);
				i++;
			}
			free(environ);
			environ = g_env_orig;
			g_env_orig = NULL;
			g_env_len = 0;
			g_env_owned = 0;
		}

		return (1);
	}

	return (0);
}

/**
 * builtin_setenv - implement setenv builtin
 * @tokens: argv (setenv VARIABLE VALUE)
 *
 * Return: 0 to continue shell (never exits)
 */
static int builtin_setenv(char **tokens)
{
	size_t i = 0, name_len;
	char *name, *value, *entry;
	char **newenv;

	if (!tokens[1] || !tokens[2] || tokens[3])
	{
		fprintf(stderr, "setenv: usage: setenv VARIABLE VALUE\n");
		return (0);
	}

	name = tokens[1];
	value = tokens[2];

	if (name[0] == '\0' || _strchr(name, '=') != NULL)
	{
		fprintf(stderr, "setenv: invalid variable name\n");
		return (0);
	}

	/* أول تعديل للبيئة: انسخ environ وصرنا نملكها */
	if (!g_env_owned)
	{
		g_env_orig = environ;
		while (environ && environ[g_env_len])
			g_env_len++;

		newenv = malloc(sizeof(char *) * (g_env_len + 1));
		if (!newenv)
		{
			fprintf(stderr, "setenv: malloc failed\n");
			return (0);
		}

		for (i = 0; i < g_env_len; i++)
		{
			newenv[i] = malloc(_strlen(environ[i]) + 1);
			if (!newenv[i])
			{
				while (i > 0)
					free(newenv[--i]);
				free(newenv);
				fprintf(stderr, "setenv: malloc failed\n");
				return (0);
			}
			_strcpy(newenv[i], environ[i]);
		}
		newenv[g_env_len] = NULL;
		environ = newenv;
		g_env_owned = 1;
	}

	name_len = _strlen(name);

	/* كوّن "NAME=VALUE" */
	entry = malloc(name_len + 1 + _strlen(value) + 1);
	if (!entry)
	{
		fprintf(stderr, "setenv: malloc failed\n");
		return (0);
	}
	_strcpy(entry, name);
	_strcat(entry, "=");
	_strcat(entry, value);

	/* إذا موجود: استبدل */
	for (i = 0; environ && environ[i]; i++)
	{
		if (_strncmp(environ[i], name, name_len) == 0 &&
		    environ[i][name_len] == '=')
		{
			free(environ[i]);
			environ[i] = entry;
			return (0);
		}
	}

	/* غير موجود: كبّر المصفوفة (بدون realloc) */
	newenv = malloc(sizeof(char *) * (g_env_len + 2));
	if (!newenv)
	{
		free(entry);
		fprintf(stderr, "setenv: malloc failed\n");
		return (0);
	}

	for (i = 0; i < g_env_len; i++)
		newenv[i] = environ[i];

	newenv[g_env_len] = entry;
	newenv[g_env_len + 1] = NULL;

	free(environ);
	environ = newenv;
	g_env_len++;

	return (0);
}

/**
 * builtin_unsetenv - implement unsetenv builtin
 * @tokens: argv (unsetenv VARIABLE)
 *
 * Return: 0 to continue shell (never exits)
 */
static int builtin_unsetenv(char **tokens)
{
	size_t i = 0, name_len;
	char *name;

	if (!tokens[1] || tokens[2])
	{
		fprintf(stderr, "unsetenv: usage: unsetenv VARIABLE\n");
		return (0);
	}

	name = tokens[1];
	if (name[0] == '\0' || _strchr(name, '=') != NULL)
	{
		fprintf(stderr, "unsetenv: invalid variable name\n");
		return (0);
	}

	/* أول تعديل للبيئة: انسخ environ وصرنا نملكها */
	if (!g_env_owned)
	{
		char **newenv;

		g_env_orig = environ;
		while (environ && environ[g_env_len])
			g_env_len++;

		newenv = malloc(sizeof(char *) * (g_env_len + 1));
		if (!newenv)
		{
			fprintf(stderr, "unsetenv: malloc failed\n");
			return (0);
		}

		for (i = 0; i < g_env_len; i++)
		{
			newenv[i] = malloc(_strlen(environ[i]) + 1);
			if (!newenv[i])
			{
				while (i > 0)
					free(newenv[--i]);
				free(newenv);
				fprintf(stderr, "unsetenv: malloc failed\n");
				return (0);
			}
			_strcpy(newenv[i], environ[i]);
		}
		newenv[g_env_len] = NULL;
		environ = newenv;
		g_env_owned = 1;
	}

	name_len = _strlen(name);

	for (i = 0; environ && environ[i]; i++)
	{
		if (_strncmp(environ[i], name, name_len) == 0 &&
		    environ[i][name_len] == '=')
		{
			size_t j;

			free(environ[i]);
			for (j = i; environ[j]; j++)
				environ[j] = environ[j + 1];

			if (g_env_len > 0)
				g_env_len--;

			return (0);
		}
	}

	/* إذا المتغير غير موجود: اعتبرها نجاح بدون طباعة */
	return (0);
}

/**
 * is_unsigned_number - check if string is digits only
 * @s: string
 *
 * Return: 1 if valid, 0 otherwise
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
 * Return: converted value
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
