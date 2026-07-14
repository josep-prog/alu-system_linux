#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 12345
#define BUF_SIZE 4096

/**
 * create_server_socket - Open an IPv4/TCP socket, bind it to
 *                        port 12345 on any address, and listen
 *
 * Return: The listening socket's file descriptor
 */
static int create_server_socket(void)
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
	addr.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 128) == -1)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	return (server_fd);
}

/**
 * main - Open an IPv4/TCP socket, bind it to port 12345 on any
 *        address, accept a single incoming connection, print the
 *        connected client's IP address, wait for a message from
 *        that client, print it, then close the connection
 *
 * Return: EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int main(void)
{
	int server_fd, client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len;
	char buffer[BUF_SIZE];
	ssize_t n;

	server_fd = create_server_socket();

	printf("Server listening on port %d\n", PORT);
	fflush(stdout);

	client_len = sizeof(client_addr);
	client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
		&client_len);
	if (client_fd == -1)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}

	printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));
	fflush(stdout);

	n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (n == -1)
	{
		perror("recv");
		exit(EXIT_FAILURE);
	}
	buffer[n] = '\0';

	printf("Message received: \"%s\"\n", buffer);
	fflush(stdout);

	close(client_fd);
	close(server_fd);

	return (EXIT_SUCCESS);
}
