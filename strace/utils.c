#include "strace.h"

/**
 * start_child - forks and execs the traced command in the child
 * @argv: NULL-terminated argument vector, argv[0] is the full binary path
 *
 * Return: the child's pid
 */
pid_t start_child(char **argv)
{
	pid_t child;

	child = fork();
	if (child == -1)
	{
		perror("fork");
		exit(1);
	}
	if (child == 0)
	{
		if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
		{
			perror("ptrace");
			exit(1);
		}
		execv(argv[0], argv);
		perror(argv[0]);
		exit(1);
	}
	waitpid(child, NULL, 0);
	return (child);
}

/**
 * wait_for_syscall - resumes the child until the next syscall-stop
 * @child: pid of the traced child
 * @status: where the wait status is stored
 *
 * Return: 1 if the child is still alive and stopped at a syscall,
 * 0 if the child has exited
 */
int wait_for_syscall(pid_t child, int *status)
{
	if (ptrace(PTRACE_SYSCALL, child, NULL, NULL) == -1)
		return (0);
	if (waitpid(child, status, 0) == -1)
		return (0);
	if (WIFEXITED(*status) || WIFSIGNALED(*status))
		return (0);
	return (1);
}
