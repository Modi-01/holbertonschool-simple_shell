#include "hsh.h"

/**
 * _strlen - returns the length of a string
 * @s: string
 *
 * Return: length of s
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
 * _strcmp - compares two strings
 * @s1: first string
 * @s2: second string
 *
 * Return: difference between first non-matching chars,
 * or 0 if equal
 */
int _strcmp(const char *s1, const char *s2)
{
	size_t i = 0;

	if (s1 == s2)
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
 * _strcpy - copies a string
 * @dest: destination buffer
 * @src: source string
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
 * _strchr - locates a character in a string
 * @s: string
 * @c: character
 *
 * Return: pointer to first occurrence of c, or NULL
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

	if ((char)c == '\0')
		return ((char *)(s + i));

	return (NULL);
}
