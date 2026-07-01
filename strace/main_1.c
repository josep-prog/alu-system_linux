#include "strace.h"

/**
 * main - executes and traces a command, printing each syscall name
 * @argc: argument count
 * @argv: argument vector, argv[1] is the full path to the binary to run
 *
 * Return: 0 on success, 1 on error
 */
int main(int argc, char **argv)
{
	pid_t child;
	int status, entering = 1;
	struct user_regs_struct regs;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
		return (1);
	}

	child = start_child(argv + 1);

	/* the stop right after execve doubles as its own syscall-stop */
	ptrace(PTRACE_GETREGS, child, NULL, &regs);
	printf("%s\n", get_syscall_name((long)regs.orig_rax));

	while (wait_for_syscall(child, &status))
	{
		if (entering)
		{
			ptrace(PTRACE_GETREGS, child, NULL, &regs);
			printf("%s\n", get_syscall_name((long)regs.orig_rax));
		}
		entering = !entering;
	}

	return (0);
}
