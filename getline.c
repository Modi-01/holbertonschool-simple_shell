#include "hsh.h"

#define READ_BUF_SIZE 1024

static ssize_t refill_buf(int fd, char *buf, size_t *len, size_t *pos);
static int grow_line(char **lineptr, size_t *cap, size_t need);

/**
 * _getline - read a line from fd using a static buffer
 * @lineptr: address of line buffer pointer
 * @n: address of buffer capacity
 * @fd: file descriptor to read from
 *
 * Return: number of bytes read (including '\n' if present), or -1 on EOF/error
 */
ssize_t _getline(char **lineptr, size_t *n, int fd)
{
	static char buf[READ_BUF_SIZE];
	static size_t len, pos;
	ssize_t r;
	size_t sz;
	char c;

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

	sz = 0;
	while (1)
	{
		if (pos >= len)
		{
			r = refill_buf(fd, buf, &len, &pos);
			if (r <= 0)
				break;
		}

		c = buf[pos++];

		if (grow_line(lineptr, n, sz + 2) == -1)
			return (-1);

		(*lineptr)[sz++] = c;
		(*lineptr)[sz] = '\0';

		if (c == '\n')
			return ((ssize_t)sz);
	}

	if (sz == 0)
		return (-1);

	return ((ssize_t)sz);
}

/**
 * refill_buf - refill static buffer
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
 * grow_line - ensure line buffer has at least need bytes
 * @lineptr: address of line buffer pointer
 * @cap: address of current capacity
 * @need: required capacity
 *
 * Return: 0 on success, -1 on failure
 */
static int grow_line(char **lineptr, size_t *cap, size_t need)
{
	char *tmp;
	size_t new_cap, i;

	if (*cap >= need)
		return (0);

	new_cap = *cap;
	while (new_cap < need)
		new_cap *= 2;

	tmp = malloc(new_cap);
	if (!tmp)
		return (-1);

	i = 0;
	while (i < *cap)
	{
		tmp[i] = (*lineptr)[i];
		if ((*lineptr)[i] == '\0')
			break;
		i++;
	}

	free(*lineptr);
	*lineptr = tmp;
	*cap = new_cap;

	return (0);
}
