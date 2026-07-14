#ifndef TODO_API_H
#define TODO_API_H

#include <stddef.h>

#define TODO_API_PORT 8080
#define TODO_API_BACKLOG 128
#define TODO_API_MAX_PAIRS 32
#define TODO_API_MAX_HEADERS 32
#define TODO_API_KEY_SIZE 256
#define TODO_API_VALUE_SIZE 1024

/**
 * struct pair_s - Generic key/value pair
 *
 * @key: Pair key
 * @value: Pair value
 */
typedef struct pair_s
{
	char key[TODO_API_KEY_SIZE];
	char value[TODO_API_VALUE_SIZE];
} pair_t;

/**
 * struct http_request_s - Parsed HTTP/1.1 request
 *
 * @method: Request method (GET, POST, ...)
 * @path: Request path, without the query string
 * @version: HTTP version (e.g. "HTTP/1.1")
 * @headers: Array of header key/value pairs
 * @header_count: Number of headers stored
 * @queries: Array of query string key/value pairs
 * @query_count: Number of query pairs stored
 * @body: Pointer to the request body (NULL terminated, may be empty)
 * @body_length: Length of the request body
 * @body_params: Array of url encoded body key/value pairs
 * @body_param_count: Number of body params stored
 */
typedef struct http_request_s
{
	char method[16];
	char path[512];
	char version[16];
	pair_t headers[TODO_API_MAX_HEADERS];
	size_t header_count;
	pair_t queries[TODO_API_MAX_PAIRS];
	size_t query_count;
	char *body;
	size_t body_length;
	pair_t body_params[TODO_API_MAX_PAIRS];
	size_t body_param_count;
} http_request_t;

/**
 * struct todo_s - A single TODO item
 *
 * @id: Positive integer identifier, starting from 0
 * @title: Title of the todo
 * @description: Description of the todo
 */
typedef struct todo_s
{
	int id;
	char *title;
	char *description;
} todo_t;

/* server.c */
int create_server_socket(int port);
void run_server(int port, void (*handler)(int, char const *));

/* request.c */
char *read_raw_request(int client_fd, size_t *out_len);

/* parse.c */
void parse_request(char *raw, http_request_t *req);

/* params.c */
size_t parse_params(char const *str, pair_t *out, size_t max);
char const *find_header(http_request_t const *req, char const *name);
char const *find_param(pair_t const *pairs, size_t count, char const *key);

/* response.c */
void send_response(int client_fd, int code, char const *reason,
	char const *content_type, char const *body, size_t body_length);
void send_simple_response(int client_fd, int code, char const *reason);
void send_json_response(int client_fd, int code, char const *reason,
	char const *json_body);

/* todo.c */
todo_t *create_todo(char const *title, char const *description);
char *todo_to_json(todo_t const *todo);
char *todos_to_json(void);

#endif /* TODO_API_H */
