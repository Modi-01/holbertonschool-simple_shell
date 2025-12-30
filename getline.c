#include "hsh.h"

#define READ_BUF_SIZE 1024

static ssize_t refill_buf(int fd, char *buf, size_t *len, size_t *pos);
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
	static size_t len, pos;
	size_t sz = 0;
	ssize_t r;

	if (!lineptr || !n || fd < 0)
		return (-1);

	if (*lineptr == NULL || *n == 0)
	{
		*n = 128;
		*lineptr = malloc(*n);
		if (!*lineptr)
			return (-1);
		(*lineptr)[0] = '\0';
	}

	while (1)
	{
		if (pos >= len)
		{
			r = refill_buf(fd, buf, &len, &pos);
			if (r <= 0)
				break;
		}

		if (append_char(lineptr, n, &sz, buf[pos++]) == -1)
			return (-1);

		if ((*lineptr)[sz - 1] == '\n')
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
static ssize_t refill_buf(int fd, char *buf, size_t *len, size_t *pos)
{
	ssize_t r;

	r = read(fd, buf, READ_BUF_SIZE);
	if (r <= 0)
		return (r);

	*len = (size_t)r;
	*pos = 0;
	return (r);
}

/**
 * append_char - append a character to line buffer, growing if needed
 * @line: pointer to line buffer pointer
 * @cap: pointer to capacity
 * @sz: pointer to current size
 * @c: char to append
 *
 * Return: 0 on success, -1 on failure
 */
static int append_char(char **line, size_t *cap, size_t *sz, char c)
{
	char *tmp;
	size_t new_cap;

	if (*sz + 1 >= *cap)
	{
		new_cap = (*cap) * 2;
		tmp = realloc(*line, new_cap);
		if (!tmp)
			return (-1);
		*line = tmp;
		*cap = new_cap;
	}

	(*line)[*sz] = c;
	(*sz)++;
	(*line)[*sz] = '\0';
	return (0);
}
