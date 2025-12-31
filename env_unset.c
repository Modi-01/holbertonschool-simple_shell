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
 * env_unset - remove an environment variable
 * @ctx: shell context
 * @name: variable name
 *
 * Return: 0 on success, -1 on failure
 */
int env_unset(shell_ctx_t *ctx, const char *name)
{
	size_t nlen, i, n;
	int idx;

	if (ctx == NULL || ctx->env == NULL || name == NULL)
		return (-1);

	if (name[0] == '\0' || strchr(name, '=') != NULL)
		return (-1);

	nlen = strlen(name);
	idx = env_find(ctx->env, name, nlen);
	if (idx < 0)
		return (0);

	free(ctx->env[idx]);

	n = env_count(ctx->env);
	for (i = (size_t)idx; i < n; i++)
		ctx->env[i] = ctx->env[i + 1];

	environ = ctx->env;
	return (0);
}
