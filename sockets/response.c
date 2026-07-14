#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "todo_api.h"

/**
 * send_response - Build and send an HTTP/1.1 response
 * @client_fd: Socket to write the response to
 * @code: HTTP status code
 * @reason: HTTP status reason phrase
 * @content_type: Value of the Content-Type header, or NULL to omit it
 * @body: Response body, or NULL if there is none
 * @body_length: Length of the response body
 */
void send_response(int client_fd, int code, char const *reason,
	char const *content_type, char const *body, size_t body_length)
{
	char header[512];
	int header_len;

	header_len = snprintf(header, sizeof(header), "HTTP/1.1 %d %s\r\n",
		code, reason);
	if (content_type)
	{
		header_len += snprintf(header + header_len,
			sizeof(header) - (size_t)header_len,
			"Content-Length: %zu\r\n", body_length);
		header_len += snprintf(header + header_len,
			sizeof(header) - (size_t)header_len,
			"Content-Type: %s\r\n", content_type);
	}
	header_len += snprintf(header + header_len,
		sizeof(header) - (size_t)header_len, "\r\n");

	send(client_fd, header, (size_t)header_len, 0);
	if (body && body_length > 0)
		send(client_fd, body, body_length, 0);
}

/**
 * send_simple_response - Send a response with no body
 * @client_fd: Socket to write the response to
 * @code: HTTP status code
 * @reason: HTTP status reason phrase
 */
void send_simple_response(int client_fd, int code, char const *reason)
{
	send_response(client_fd, code, reason, NULL, NULL, 0);
}

/**
 * send_json_response - Send a response with a JSON body
 * @client_fd: Socket to write the response to
 * @code: HTTP status code
 * @reason: HTTP status reason phrase
 * @json_body: NULL terminated JSON body
 */
void send_json_response(int client_fd, int code, char const *reason,
	char const *json_body)
{
	send_response(client_fd, code, reason, "application/json",
		json_body, strlen(json_body));
}
