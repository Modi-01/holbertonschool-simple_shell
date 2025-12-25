#include "hsh.h"

/**
 * main - Entry point for hsh shell
 * @ac: argument count
 * @av: argument vector
 *
 * Return: exit status
 */
int main(int ac, char **av)
{
	(void)ac;
	return (run_shell(av[0]));
}
