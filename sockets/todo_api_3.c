#include <stdio.h>
#include <stdlib.h>
#include "todo_api.h"

/**
 * handle_connection - Read an HTTP request, print its raw content
 *                      and the path and body parameter key/value
 *                      pairs, then reply with a 200 OK
 * @client_fd: Connected client socket
 * @client_ip: Connected client's IP address (unused here)
 */
static void handle_connection(int client_fd, char const *client_ip)
{
	char *raw;
	size_t raw_len, i;
	http_request_t req;

	(void)client_ip;

	raw = read_raw_request(client_fd, &raw_len);
	printf("Raw request: \"%s\"\n", raw);

	parse_request(raw, &req);
	printf("Path: %s\n", req.path);
	for (i = 0; i < req.body_param_count; i++)
		printf("Body param: \"%s\" -> \"%s\"\n",
			req.body_params[i].key, req.body_params[i].value);

	send_simple_response(client_fd, 200, "OK");

	free(raw);
}

/**
 * main - Entry point: run the todo API server on port 8080
 *
 * Return: EXIT_SUCCESS
 */
int main(void)
{
	run_server(TODO_API_PORT, handle_connection);

	return (EXIT_SUCCESS);
}
