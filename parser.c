#include "hsh.h"

/**
 * split_line - split a line into tokens (supports arguments)
 * @line: input line (modified in-place)
 *
 * Return: NULL-terminated array of tokens or NULL
 */
char **split_line(char *line)
{
	char *copy, *tok;
	size_t count = 0, i = 0;
	char **tokens;

	if (!line)
		return (NULL);

	copy = malloc(_strlen(line) + 1);
	if (!copy)
		return (NULL);
	_strcpy(copy, line);

	tok = strtok(copy, " \t");
	while (tok)
	{
		count++;
		tok = strtok(NULL, " \t");
	}

	tokens = malloc(sizeof(char *) * (count + 1));
	if (!tokens)
	{
		free(copy);
		return (NULL);
	}

	tok = strtok(line, " \t");
	while (tok)
	{
		tokens[i++] = tok;
		tok = strtok(NULL, " \t");
	}
	tokens[i] = NULL;

	free(copy);
	return (tokens);
}

/**
 * free_tokens - free tokens array
 * @tokens: array returned by split_line
 */
void free_tokens(char **tokens)
{
	free(tokens);
}
