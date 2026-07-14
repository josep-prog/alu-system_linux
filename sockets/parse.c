#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "todo_api.h"

/**
 * parse_request_line - Parse the first line of an HTTP request
 *                       ("METHOD PATH HTTP/x.y") into a request
 * @line: NULL terminated, mutable copy of the request line
 * @req: Request structure to fill in
 */
static void parse_request_line(char *line, http_request_t *req)
{
	char *path, *path_end, *version, *query;

	path = strchr(line, ' ');
	*path = '\0';
	snprintf(req->method, sizeof(req->method), "%s", line);
	path++;

	path_end = strchr(path, ' ');
	*path_end = '\0';
	version = path_end + 1;

	query = strchr(path, '?');
	if (query)
	{
		*query = '\0';
		req->query_count = parse_params(query + 1, req->queries,
			TODO_API_MAX_PAIRS);
	}

	snprintf(req->path, sizeof(req->path), "%s", path);
	snprintf(req->version, sizeof(req->version), "%s", version);
}

/**
 * parse_headers - Parse the "Key: Value" header lines of a request
 * @block: NULL terminated, mutable block holding the header lines
 * @req: Request structure to fill in
 */
static void parse_headers(char *block, http_request_t *req)
{
	char *line, *saveptr, *colon;

	for (line = strtok_r(block, "\r\n", &saveptr);
		line && req->header_count < TODO_API_MAX_HEADERS;
		line = strtok_r(NULL, "\r\n", &saveptr))
	{
		colon = strchr(line, ':');
		if (!colon)
			continue;
		*colon = '\0';
		snprintf(req->headers[req->header_count].key,
			TODO_API_KEY_SIZE, "%s", line);
		colon++;
		while (*colon == ' ')
			colon++;
		snprintf(req->headers[req->header_count].value,
			TODO_API_VALUE_SIZE, "%s", colon);
		req->header_count++;
	}
}

/**
 * parse_request - Parse a raw HTTP request into its components
 * @raw: NULL terminated buffer holding the full raw request
 * @req: Request structure to fill in
 */
void parse_request(char *raw, http_request_t *req)
{
	char *work, *line_end, *header_block, *body_start;
	char const *content_length;

	memset(req, 0, sizeof(*req));

	work = strdup(raw);
	line_end = strstr(work, "\r\n");
	if (line_end)
		*line_end = '\0';
	parse_request_line(work, req);

	header_block = line_end ? line_end + 2 : work + strlen(work);
	parse_headers(header_block, req);
	free(work);

	body_start = strstr(raw, "\r\n\r\n");
	if (body_start)
	{
		req->body = body_start + 4;
		content_length = find_header(req, "Content-Length");
		req->body_length = content_length ?
			(size_t)strtoul(content_length, NULL, 10) : 0;
	}
	else
	{
		req->body = raw + strlen(raw);
		req->body_length = 0;
	}

	if (req->body_length > 0)
		req->body_param_count = parse_params(req->body,
			req->body_params, TODO_API_MAX_PAIRS);
}
