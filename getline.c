#include "hsh.h"

#define READ_BUF_SIZE 1024

static ssize_t refill_buf(int fd, char *buf, ssize_t *len, ssize_t *pos);
static int append_char(char **line, size_t *cap, size_t *sz, char c);

/**
 * _getline - read a line from fd using a static buffer
 * @lineptr: address of buffer pointer
 * @n: address of buffer capacity
 * @fd: file descriptor to read from
 *
 * Return: number of bytes read (including '\n' if present), or -1 on EOF/error
 */
ssize_t _getline(char **lineptr, size_t *n, int fd)
{
	static char buf[READ_BUF_SIZE];
	static ssize_t len;
	static ssize_t pos;
	size_t cap, sz;
	ssize_t r;
	char *line;

	if (!lineptr || !n || fd < 0)
		return (-1);

	line = *lineptr;
	cap = *n;
	sz = 0;

	if (!line || cap == 0)
	{
		cap = 128;
		line = malloc(cap);
		if (!line)
			return (-1);
		line[0] = '\0';
		*lineptr = line;
		*n = cap;
	}

	while (1)
	{
		if (pos >= len)
		{
			r = refill_buf(fd, buf, &len, &pos);
			if (r <= 0)
				break;
		}

		if (append_char(&line, &cap, &sz, buf[pos++]) == -1)
			return (-1);

		*lineptr = line;
		*n = cap;

		if (sz > 0 && line[sz - 1] == '\n')
			return ((ssize_t)sz);
	}

	if (sz == 0)
		return (-1);

	return ((ssize_t)sz);
}

/**
 * refill_buf - refill static buffer using read
 * @fd: file descriptor
 * @buf: buffer
 * @len: current valid length
 * @pos: current position
 *
 * Return: bytes read, 0 on EOF, -1 on error
 */
static ssize_t refill_buf(int fd, char *buf, ssize_t *len, ssize_t *pos)
{
	ssize_t r;

	r = read(fd, buf, READ_BUF_SIZE);
	if (r <= 0)
		return (r);

	*len = r;
	*pos = 0;
	return (r);
}

/**
 * append_char - append a character to line buffer, growing if needed
 * @line: pointer to line buffer
 * @cap: pointer to capacity
 * @sz: pointer to current size (excluding null)
 * @c: char to append
 *
 * Return: 0 on success, -1 on failure
 */
static int append_char(char **line, size_t *cap, size_t *sz, char c)
{
	char *new_line;
	size_t new_cap, i;

	if (*sz + 2 > *cap)
	{
		new_cap = (*cap) * 2;
		new_line = malloc(new_cap);
		if (!new_line)
			return (-1);

		for (i = 0; i < *sz; i++)
			new_line[i] = (*line)[i];

		new_line[*sz] = '\0';
		free(*line);
		*line = new_line;
		*cap = new_cap;
	}

	(*line)[*sz] = c;
	(*sz)++;
	(*line)[*sz] = '\0';
	return (0);
}
