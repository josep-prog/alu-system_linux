#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "todo_api.h"

#define READ_CHUNK 4096

/**
 * find_double_crlf - Locate the blank line that separates the HTTP
 *                     headers from the body
 * @buf: NULL terminated buffer received so far
 *
 * Return: Offset of the first byte of the body, or -1 if the
 *         headers are not complete yet
 */
static long find_double_crlf(char const *buf)
{
	char const *found;

	found = strstr(buf, "\r\n\r\n");
	if (!found)
		return (-1);
	return ((long)(found - buf) + 4);
}

/**
 * extract_content_length - Look up the Content-Length header value
 * @buf: NULL terminated buffer holding at least the request headers
 *
 * Return: The announced body length, or -1 if the header is absent
 */
static long extract_content_length(char const *buf)
{
	char const *found;

	found = strstr(buf, "Content-Length:");
	if (!found)
		return (-1);
	found += strlen("Content-Length:");
	while (*found == ' ')
		found++;
	return (strtol(found, NULL, 10));
}

/**
 * read_raw_request - Read a full HTTP request (headers and, if
 *                     announced, body) from a client socket
 * @client_fd: Socket to read from
 * @out_len: Where to store the total number of bytes read
 *
 * Return: A malloc'd, NULL terminated buffer holding the raw request
 */
char *read_raw_request(int client_fd, size_t *out_len)
{
	char *buf;
	size_t capacity, len;
	long body_start, content_length, needed;
	ssize_t n;

	capacity = READ_CHUNK;
	len = 0;
	buf = malloc(capacity);
	body_start = -1;

	while (1)
	{
		if (capacity - len < READ_CHUNK)
		{
			capacity *= 2;
			buf = realloc(buf, capacity);
		}

		n = recv(client_fd, buf + len, capacity - len - 1, 0);
		if (n <= 0)
			break;
		len += (size_t)n;
		buf[len] = '\0';

		if (body_start == -1)
			body_start = find_double_crlf(buf);
		if (body_start != -1)
		{
			content_length = extract_content_length(buf);
			if (content_length <= 0)
				break;
			needed = body_start + content_length;
			if ((long)len >= needed)
				break;
		}
	}

	*out_len = len;
	return (buf);
}
