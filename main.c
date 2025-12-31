#include "hsh.h"

/**
 * main - entry point
 * @ac: argument count
 * @av: argument vector
 *
 * Return: 0 on success
 */
int main(int ac, char **av)
{
	shell_ctx_t ctx;

	(void)ac;
	ctx.av0 = av[0];
	ctx.line_num = 0;
	ctx.interactive = isatty(STDIN_FILENO);
	ctx.should_exit = 0;
	ctx.exit_status = 0;
	ctx.env = NULL;

	if (env_init(&ctx) == -1)
		return (1);

	shell_loop(&ctx);

	env_free(&ctx);

	return (0);
}
