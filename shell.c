#include "hsh.h"

/**
 * print_prompt - display shell prompt
 *
 * Return: void
 */
static void print_prompt(void)
{
	write(STDOUT_FILENO, "($) ", 4);
}

/**
 * strip_trailing - remove trailing whitespace/newline
 * @line: input buffer
 *
 * Return: void
 */
static void strip_trailing(char *line)
{
	size_t len;

	if (line == NULL)
		return;

	len = strlen(line);
	while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == ' ' ||
			   line[len - 1] == '\t' || line[len - 1] == '\r'))
	{
		line[len - 1] = '\0';
		len--;
	}
}

/**
 * shell_loop - main loop for the shell
 * @ctx: shell context
 *
 * Return: void
 */
void shell_loop(shell_ctx_t *ctx)
{
	char *line;
	size_t cap;
	ssize_t nread;
	char **argv;

	line = NULL;
	cap = 0;

	signal(SIGINT, sigint_handler);

	while (1)
	{
		if (ctx->interactive)
			print_prompt();

		nread = my_getline(&line, &cap, STDIN_FILENO);
		if (nread == -1)
		{
			if (errno == EINTR)
				continue;

			if (ctx->interactive)
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		ctx->line_num++;
		strip_trailing(line);

		argv = split_args(line);
		if (argv == NULL || argv[0] == NULL)
		{
			free_args(argv);
			continue;
		}

		if (handle_builtins(ctx, argv) == 1)
		{
			free_args(argv);
			if (ctx->should_exit)
				break;
			continue;
		}

		(void)execute_argv(ctx, argv);
		free_args(argv);
	}

	free(line);

	if (ctx->should_exit)
	{
		env_free(ctx);
		exit(ctx->exit_status);
	}
}
