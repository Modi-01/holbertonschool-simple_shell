#include "hsh.h"

/**
 * _strlen - return length of string
 * @s: string
 *
 * Return: length
 */
size_t _strlen(const char *s)
{
	size_t len = 0;

	if (!s)
		return (0);

	while (s[len])
		len++;

	return (len);
}

/**
 * _strcmp - compare two strings
 * @s1: first string
 * @s2: second string
 *
 * Return: difference
 */
int _strcmp(const char *s1, const char *s2)
{
	size_t i = 0;

	if (!s1 && !s2)
		return (0);
	if (!s1)
		return (-1);
	if (!s2)
		return (1);

	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;

	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

/**
 * _strncmp - compare two strings up to n bytes
 * @s1: first string
 * @s2: second string
 * @n: max bytes
 *
 * Return: difference
 */
int _strncmp(const char *s1, const char *s2, size_t n)
{
	size_t i = 0;

	if (n == 0)
		return (0);

	if (!s1 && !s2)
		return (0);
	if (!s1)
		return (-1);
	if (!s2)
		return (1);

	while (i < n && s1[i] && s2[i] && s1[i] == s2[i])
		i++;

	if (i == n)
		return (0);

	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

/**
 * _strcpy - copy string
 * @dest: destination
 * @src: source
 *
 * Return: dest
 */
char *_strcpy(char *dest, const char *src)
{
	size_t i = 0;

	if (!dest || !src)
		return (dest);

	while (src[i])
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';

	return (dest);
}

/**
 * _strcat - concatenate src to dest
 * @dest: destination (must have enough space)
 * @src: source
 *
 * Return: dest
 */
char *_strcat(char *dest, const char *src)
{
	size_t i = 0, j = 0;

	if (!dest || !src)
		return (dest);

	while (dest[i])
		i++;

	while (src[j])
	{
		dest[i + j] = src[j];
		j++;
	}
	dest[i + j] = '\0';

	return (dest);
}

/**
 * _strchr - locate char in string
 * @s: string
 * @c: char
 *
 * Return: pointer or NULL
 */
char *_strchr(const char *s, int c)
{
	size_t i = 0;

	if (!s)
		return (NULL);

	while (s[i])
	{
		if (s[i] == (char)c)
			return ((char *)(s + i));
		i++;
	}

	if (c == '\0')
		return ((char *)(s + i));

	return (NULL);
}
