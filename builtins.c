#include "hsh.h"

/**
 * is_number - check if a string is a valid non-negative integer
 * @s: string to check
 *
 * Return: 1 if number, 0 otherwise
 */
static int is_number(const char *s)
{
	size_t i;

	if (s == NULL || s[0] == '\0')
		return (0);

	for (i = 0; s[i] != '\0'; i++)
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
	}

	return (1);
}

/**
 * to_exit_status - convert numeric string to exit status (0-255)
 * @s: numeric string
 *
 * Return: exit status
 */
static int to_exit_status(const char *s)
{
	unsigned long n;
	size_t i;

	n = 0;
	for (i = 0; s[i] != '\0'; i++)
		n = n * 10 + (unsigned long)(s[i] - '0');

	return ((int)(n % 256));
}

/**
 * builtin_error - print builtin failure message to stderr
 * @ctx: shell context
 * @name: builtin name
 * @msg: message
 *
 * Return: void
 */
static void builtin_error(shell_ctx_t *ctx, const char *name, const char *msg)
{
	dprintf(STDERR_FILENO, "%s: %lu: %s: %s\n",
		ctx->av0, ctx->line_num, name, msg);
}

/**
 * handle_builtins - handle built-in commands
 * @ctx: shell context
 * @argv: command argv
 *
 * Return: 1 if handled as builtin, 0 otherwise
 */
int handle_builtins(shell_ctx_t *ctx, char **argv)
{
	if (argv == NULL || argv[0] == NULL)
		return (0);

	if (strcmp(argv[0], "exit") == 0)
	{
		ctx->should_exit = 1;
		ctx->exit_status = 0;

		if (argv[1] != NULL && is_number(argv[1]))
			ctx->exit_status = to_exit_status(argv[1]);

		return (1);
	}

	if (strcmp(argv[0], "setenv") == 0)
	{
		if (argv[1] == NULL || argv[2] == NULL || argv[3] != NULL)
		{
			builtin_error(ctx, "setenv", "usage: setenv VARIABLE VALUE");
			return (1);
		}
		if (env_set(ctx, argv[1], argv[2]) == -1)
		{
			builtin_error(ctx, "setenv", "failed");
			return (1);
		}
		return (1);
	}

	if (strcmp(argv[0], "unsetenv") == 0)
	{
		if (argv[1] == NULL || argv[2] != NULL)
		{
			builtin_error(ctx, "unsetenv", "usage: unsetenv VARIABLE");
			return (1);
		}
		if (env_unset(ctx, argv[1]) == -1)
		{
			builtin_error(ctx, "unsetenv", "failed");
			return (1);
		}
		return (1);
	}

	return (0);
}
