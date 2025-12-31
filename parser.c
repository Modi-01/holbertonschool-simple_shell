#include "hsh.h"

/**
 * is_delim - check if char is a delimiter (space or tab)
 * @c: character
 *
 * Return: 1 if delimiter, 0 otherwise
 */
static int is_delim(char c)
{
	return (c == ' ' || c == '\t');
}

/**
 * count_tokens - count tokens in a line (space/tab separated)
 * @line: input line
 *
 * Return: number of tokens
 */
static size_t count_tokens(char *line)
{
	size_t i = 0, count = 0;

	if (!line)
		return (0);

	while (line[i])
	{
		while (line[i] && is_delim(line[i]))
			i++;

		if (!line[i])
			break;

		count++;

		while (line[i] && !is_delim(line[i]))
			i++;
	}

	return (count);
}

/**
 * split_line - split a line into tokens (supports arguments)
 * @line: input line (modified in-place)
 *
 * Return: NULL-terminated array of tokens or NULL
 */
char **split_line(char *line)
{
	size_t i = 0, j = 0, count;
	char **tokens;

	if (!line)
		return (NULL);

	count = count_tokens(line);
	tokens = malloc(sizeof(char *) * (count + 1));
	if (!tokens)
		return (NULL);

	while (line[i])
	{
		while (line[i] && is_delim(line[i]))
			i++;

		if (!line[i])
			break;

		tokens[j++] = &line[i];

		while (line[i] && !is_delim(line[i]))
			i++;

		if (line[i])
		{
			line[i] = '\0';
			i++;
		}
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
