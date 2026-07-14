#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

/**
 * connect_to_server - Resolve a host/port and connect to it over TCP
 * @host: Host to connect to
 * @port: Port to connect to
 *
 * Return: The connected socket's file descriptor
 */
static int connect_to_server(char const *host, char const *port)
{
	int sock_fd;
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(host, port, &hints, &res) != 0)
	{
		fprintf(stderr, "Could not resolve host: %s\n", host);
		exit(EXIT_FAILURE);
	}

	sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock_fd == -1)
	{
		perror("socket");
		freeaddrinfo(res);
		exit(EXIT_FAILURE);
	}

	if (connect(sock_fd, res->ai_addr, res->ai_addrlen) == -1)
	{
		perror("connect");
		freeaddrinfo(res);
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(res);

	return (sock_fd);
}

/**
 * main - Connect to a listening TCP server
 * @argc: Argument count
 * @argv: Argument vector: program name, host, port
 *
 * Return: EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int main(int argc, char *argv[])
{
	int sock_fd;

	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	sock_fd = connect_to_server(argv[1], argv[2]);

	printf("Connected to %s:%s\n", argv[1], argv[2]);

	close(sock_fd);

	return (EXIT_SUCCESS);
}
