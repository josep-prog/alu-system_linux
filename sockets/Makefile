CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

COMMON_SRC = server.c request.c parse.c params.c response.c todo.c

all: 0-server 1-server 2-client 3-server \
	todo_api_0 todo_api_1 todo_api_2 todo_api_3 todo_api_4 todo_api_5

0-server: 0-server.c
	$(CC) $(CFLAGS) 0-server.c -o 0-server

1-server: 1-server.c
	$(CC) $(CFLAGS) 1-server.c -o 1-server

2-client: 2-client.c
	$(CC) $(CFLAGS) 2-client.c -o 2-client

3-server: 3-server.c
	$(CC) $(CFLAGS) 3-server.c -o 3-server

todo_api_0: $(COMMON_SRC) todo_api_0.c
	$(CC) $(CFLAGS) $(COMMON_SRC) todo_api_0.c -o todo_api_0

todo_api_1: $(COMMON_SRC) todo_api_1.c
	$(CC) $(CFLAGS) $(COMMON_SRC) todo_api_1.c -o todo_api_1

todo_api_2: $(COMMON_SRC) todo_api_2.c
	$(CC) $(CFLAGS) $(COMMON_SRC) todo_api_2.c -o todo_api_2

todo_api_3: $(COMMON_SRC) todo_api_3.c
	$(CC) $(CFLAGS) $(COMMON_SRC) todo_api_3.c -o todo_api_3

todo_api_4: $(COMMON_SRC) todo_api_4.c
	$(CC) $(CFLAGS) $(COMMON_SRC) todo_api_4.c -o todo_api_4

todo_api_5: $(COMMON_SRC) todo_api_5.c
	$(CC) $(CFLAGS) $(COMMON_SRC) todo_api_5.c -o todo_api_5

clean:
	rm -f 0-server 1-server 2-client 3-server \
		todo_api_0 todo_api_1 todo_api_2 todo_api_3 todo_api_4 todo_api_5

.PHONY: all clean
