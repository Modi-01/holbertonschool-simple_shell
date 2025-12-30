#include "hsh.h"

/**
 * main - Entry point
 * @ac: argument count
 * @av: argument vector
 *
 * Return: shell status
 */
int main(int ac, char **av)
{
	(void)ac;
	return (run_shell(av[0]));
}
