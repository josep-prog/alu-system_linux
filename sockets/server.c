#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "todo_api.h"

/**
 * create_server_socket - Open an IPv4/TCP socket, bind it to the
 *                        given port on any address, and listen
 * @port: Port to listen on
 *
 * Return: The listening socket's file descriptor
 */
int create_server_socket(int port)
{
	int server_fd, opt;
	struct sockaddr_in addr;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
		&opt, sizeof(opt)) == -1)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, TODO_API_BACKLOG) == -1)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	return (server_fd);
}

/**
 * run_server - Create a listening socket and forever accept
 *              connections, handing each one to a handler
 * @port: Port to listen on
 * @handler: Callback invoked with the client socket and IP string
 */
void run_server(int port, void (*handler)(int, char const *))
{
	int server_fd, client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len;
	char const *client_ip;

	setvbuf(stdout, NULL, _IOLBF, BUFSIZ);

	server_fd = create_server_socket(port);
	printf("Server listening on port %d\n", port);
	fflush(stdout);

	while (1)
	{
		client_len = sizeof(client_addr);
		client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
			&client_len);
		if (client_fd == -1)
		{
			perror("accept");
			continue;
		}

		client_ip = inet_ntoa(client_addr.sin_addr);
		printf("Client connected: %s\n", client_ip);
		fflush(stdout);

		handler(client_fd, client_ip);

		close(client_fd);
	}
}
