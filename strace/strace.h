#ifndef STRACE_H
#define STRACE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>

/**
 * struct syscall_args_t - describes how many arguments a syscall takes
 * @number: the syscall number
 * @nargs: number of arguments to print in hexadecimal (0 to 6)
 * @variadic: 1 if a "..." must be appended after the printed arguments
 */
typedef struct syscall_args_t
{
	long number;
	int nargs;
	int variadic;
} syscall_args_t;

pid_t start_child(char **argv);
int wait_for_syscall(pid_t child, int *status);

const char *get_syscall_name(long number);
void get_syscall_args(long number, int *nargs, int *variadic);

#endif /* STRACE_H */
