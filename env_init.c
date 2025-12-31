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
 * env_init - create an owned copy of environ for this shell
 * @ctx: shell context
 *
 * Return: 0 on success, -1 on failure
 */
int env_init(shell_ctx_t *ctx)
{
	size_t n, i;

	if (ctx == NULL)
		return (-1);

	n = env_count(environ);
	ctx->env = malloc(sizeof(char *) * (n + 1));
	if (ctx->env == NULL)
		return (-1);

	for (i = 0; i < n; i++)
	{
		ctx->env[i] = strdup(environ[i]);
		if (ctx->env[i] == NULL)
		{
			while (i > 0)
				free(ctx->env[--i]);
			free(ctx->env);
			ctx->env = NULL;
			return (-1);
		}
	}
	ctx->env[n] = NULL;

	environ = ctx->env;
	return (0);
}

/**
 * env_free - free owned environment copy
 * @ctx: shell context
 *
 * Return: void
 */
void env_free(shell_ctx_t *ctx)
{
	size_t i;

	if (ctx == NULL || ctx->env == NULL)
		return;

	for (i = 0; ctx->env[i] != NULL; i++)
		free(ctx->env[i]);

	free(ctx->env);
	ctx->env = NULL;
}
