# Sockets

Low-level C project exploring BSD sockets: opening an IPv4/TCP socket,
binding and listening on a port, accepting connections, and reading
data from a client. Culminates in a small HTTP/1.1 REST API server
(a RAM-only TODO list) built directly on top of `socket`/`bind`/
`listen`/`accept`/`recv`/`send`, with no external HTTP library.

## Learning Objectives

* What is a socket and how it is represented on a Linux/UNIX system
* The different types of sockets and socket domains
* How to create, bind, listen and accept on a socket
* How to connect to a remote application
* What the HTTP protocol is
* How to build a simple HTTP server and a small REST API by hand

## Files

| File | Description |
| --- | --- |
| `0-server.c` | Opens a socket, binds/listens on port 12345, hangs forever |
| `1-server.c` | Accepts a single connection and prints the client's IP |
| `2-client.c` | Connects to `<host> <port>` and prints a confirmation |
| `3-server.c` | Accepts a connection, receives and prints one message |
| `todo_api.h` | Shared types/prototypes for the REST API tasks |
| `server.c` | Listening socket setup + generic accept loop (`run_server`) |
| `request.c` | Reads a full raw HTTP request off the wire (headers + body) |
| `parse.c` | Parses the raw request into method/path/version/headers/body |
| `params.c` | `key=value&...` parsing, header/param lookup helpers |
| `response.c` | Builds and sends HTTP/1.1 responses (plain or JSON) |
| `todo.c` | In-memory TODO list and its JSON serialization |
| `todo_api_0.c` | Prints the raw request and its request-line breakdown |
| `todo_api_1.c` | Prints the path and query string key/value pairs |
| `todo_api_2.c` | Prints the request headers |
| `todo_api_3.c` | Prints the path and url-encoded body parameters |
| `todo_api_4.c` | `POST /todos` (create), 404 for anything else |
| `todo_api_5.c` | Adds `GET /todos` (retrieve all) |

## Compilation

```sh
make               # builds every task
make todo_api_5    # builds a single rule
make clean
```

`0-server.c`, `1-server.c`, `2-client.c` and `3-server.c` are each
self-contained (single translation unit). The `todo_api_*` targets
share `server.c request.c parse.c params.c response.c todo.c` and
listen on port 8080 instead of 12345.

## REST API

The TODO list lives entirely in memory (no persistence); it is reset
every time the server restarts. A todo has an `id` (starting at 0),
a `title` and a `description`.

| Method | Path | Behaviour |
| --- | --- | --- |
| `POST` | `/todos` | Create a todo from `title`/`description` form body params. `201` on success, `411` if `Content-Length` is missing, `422` if `title` or `description` is missing |
| `GET` | `/todos` | Return the JSON array of every todo (`200`) |

Any other method/path combination returns `404 Not Found`.

```sh
./todo_api_5
curl -X POST 'http://localhost:8080/todos' -d 'title=Dishes&description=Not_really_urgent'
curl 'http://localhost:8080/todos'
```
