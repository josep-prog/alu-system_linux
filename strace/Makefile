CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

COMMON_SRC = utils.c get_syscall_name.c syscall_args.c

all: strace_0 strace_1 strace_2 strace_3

strace_0: $(COMMON_SRC) main_0.c
	$(CC) $(CFLAGS) $(COMMON_SRC) main_0.c -o strace_0

strace_1: $(COMMON_SRC) main_1.c
	$(CC) $(CFLAGS) $(COMMON_SRC) main_1.c -o strace_1

strace_2: $(COMMON_SRC) main_2.c
	$(CC) $(CFLAGS) $(COMMON_SRC) main_2.c -o strace_2

strace_3: $(COMMON_SRC) main_3.c
	$(CC) $(CFLAGS) $(COMMON_SRC) main_3.c -o strace_3

clean:
	rm -f strace_0 strace_1 strace_2 strace_3

.PHONY: all clean
