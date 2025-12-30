#include "hsh.h"

/**
 * split_line - split a line into tokens (supports arguments)
 * @line: input line (modified in-place)
 *
 * Return: NULL-terminated array of tokens or NULL
 */
char **split_line(char *line)
{
	size_t i = 0, count = 0, j = 0;
	char **tokens;

	if (!line)
		return (NULL);

	/* first pass: count tokens */
	while (line[i])
	{
		while (line[i] == ' ' || line[i] == '\t')
			i++;

		if (line[i] == '\0')
			break;

		count++;

		while (line[i] && line[i] != ' ' && line[i] != '\t')
			i++;
	}

	tokens = malloc(sizeof(char *) * (count + 1));
	if (!tokens)
		return (NULL);

	/* second pass: store pointers and null-terminate tokens */
	i = 0;
	while (line[i])
	{
		while (line[i] == ' ' || line[i] == '\t')
		{
			line[i] = '\0';
			i++;
		}

		if (line[i] == '\0')
			break;

		tokens[j++] = &line[i];

		while (line[i] && line[i] != ' ' && line[i] != '\t')
			i++;
	}

	tokens[j] = NULL;
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
