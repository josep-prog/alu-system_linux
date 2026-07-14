#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "todo_api.h"

/**
 * parse_params - Split a "key=value&key2=value2" encoded string
 *                into an array of key/value pairs
 * @str: String to split (may be NULL or empty)
 * @out: Array to fill with the parsed pairs
 * @max: Maximum number of pairs @out can hold
 *
 * Return: The number of pairs stored in @out
 */
size_t parse_params(char const *str, pair_t *out, size_t max)
{
	char *copy, *saveptr, *token, *eq;
	size_t count;

	if (!str || !*str)
		return (0);

	copy = strdup(str);
	count = 0;

	for (token = strtok_r(copy, "&", &saveptr); token && count < max;
		token = strtok_r(NULL, "&", &saveptr))
	{
		eq = strchr(token, '=');
		if (eq)
		{
			*eq = '\0';
			snprintf(out[count].key, TODO_API_KEY_SIZE, "%s", token);
			snprintf(out[count].value, TODO_API_VALUE_SIZE, "%s", eq + 1);
		}
		else
		{
			snprintf(out[count].key, TODO_API_KEY_SIZE, "%s", token);
			out[count].value[0] = '\0';
		}
		count++;
	}

	free(copy);
	return (count);
}

/**
 * find_header - Look up a header by name, case insensitively
 * @req: Parsed request to search
 * @name: Header name to look for
 *
 * Return: The header's value, or NULL if it is not present
 */
char const *find_header(http_request_t const *req, char const *name)
{
	size_t i;

	for (i = 0; i < req->header_count; i++)
		if (strcasecmp(req->headers[i].key, name) == 0)
			return (req->headers[i].value);
	return (NULL);
}

/**
 * find_param - Look up a key/value pair by key
 * @pairs: Array of pairs to search
 * @count: Number of pairs in the array
 * @key: Key to look for
 *
 * Return: The matching value, or NULL if @key is not present
 */
char const *find_param(pair_t const *pairs, size_t count, char const *key)
{
	size_t i;

	for (i = 0; i < count; i++)
		if (strcmp(pairs[i].key, key) == 0)
			return (pairs[i].value);
	return (NULL);
}
