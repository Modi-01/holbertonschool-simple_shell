#include "hsh.h"

/**
 * env_count - count environment variables
 * @envp: environment array
 *
 * Return: number of entries
 */
static size_t env_count(char **envp)
{
	size_t n;

	if (envp == NULL)
		return (0);

	for (n = 0; envp[n] != NULL; n++)
		;

	return (n);
}

/**
 * env_find - find variable index in env
 * @envp: environment array
 * @name: variable name
 * @nlen: length of name
 *
 * Return: index if found, -1 otherwise
 */
static int env_find(char **envp, const char *name, size_t nlen)
{
	size_t i;

	for (i = 0; envp[i] != NULL; i++)
	{
		if (strncmp(envp[i], name, nlen) == 0 && envp[i][nlen] == '=')
			return ((int)i);
	}

	return (-1);
}

/**
 * build_kv - build "NAME=VALUE"
 * @name: variable name
 * @value: variable value
 *
 * Return: malloc'd string or NULL
 */
static char *build_kv(const char *name, const char *value)
{
	size_t len;
	char *s;

	len = strlen(name) + 1 + strlen(value) + 1;
	s = malloc(len);
	if (s == NULL)
		return (NULL);

	snprintf(s, len, "%s=%s", name, value);
	return (s);
}

/**
 * env_set - set or update an environment variable
 * @ctx: shell context
 * @name: variable name
 * @value: variable value
 *
 * Return: 0 on success, -1 on failure
 */
int env_set(shell_ctx_t *ctx, const char *name, const char *value)
{
	size_t n, nlen, i;
	int idx;
	char *kv;
	char **new_env;

	if (ctx == NULL || ctx->env == NULL || name == NULL || value == NULL)
		return (-1);

	if (name[0] == '\0' || strchr(name, '=') != NULL)
		return (-1);

	nlen = strlen(name);
	idx = env_find(ctx->env, name, nlen);
	kv = build_kv(name, value);
	if (kv == NULL)
		return (-1);

	if (idx >= 0)
	{
		free(ctx->env[idx]);
		ctx->env[idx] = kv;
		environ = ctx->env;
		return (0);
	}

	n = env_count(ctx->env);
	new_env = malloc(sizeof(char *) * (n + 2));
	if (new_env == NULL)
	{
		free(kv);
		return (-1);
	}

	for (i = 0; i < n; i++)
		new_env[i] = ctx->env[i];

	new_env[n] = kv;
	new_env[n + 1] = NULL;

	free(ctx->env);
	ctx->env = new_env;
	environ = ctx->env;

	return (0);
}
