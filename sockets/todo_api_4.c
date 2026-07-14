#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "todo_api.h"

/**
 * handle_post_todos - Handle "POST /todos": create a new todo
 * @client_fd: Connected client socket
 * @req: Parsed HTTP request
 *
 * Return: The HTTP status code sent back to the client
 */
static int handle_post_todos(int client_fd, http_request_t const *req)
{
	char const *title, *description;
	todo_t *todo;
	char *json;

	if (!find_header(req, "Content-Length"))
	{
		send_simple_response(client_fd, 411, "Length Required");
		return (411);
	}

	title = find_param(req->body_params, req->body_param_count, "title");
	description = find_param(req->body_params, req->body_param_count,
		"description");
	if (!title || !description)
	{
		send_simple_response(client_fd, 422, "Unprocessable Entity");
		return (422);
	}

	todo = create_todo(title, description);
	json = todo_to_json(todo);
	send_json_response(client_fd, 201, "Created", json);
	free(json);

	return (201);
}

/**
 * handle_connection - Read an HTTP request, route it to the right
 *                      handler, and log the outcome
 * @client_fd: Connected client socket
 * @client_ip: Connected client's IP address
 */
static void handle_connection(int client_fd, char const *client_ip)
{
	char *raw;
	size_t raw_len;
	http_request_t req;
	int code;

	raw = read_raw_request(client_fd, &raw_len);
	parse_request(raw, &req);

	if (strcmp(req.method, "POST") == 0 && strcmp(req.path, "/todos") == 0)
		code = handle_post_todos(client_fd, &req);
	else
	{
		send_simple_response(client_fd, 404, "Not Found");
		code = 404;
	}

	printf("%s %s %s -> %d\n", client_ip, req.method, req.path, code);

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
