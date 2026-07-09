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

	/* keep our output in sync with the tracee's own raw writes to fd 1 */
	setbuf(stdout, NULL);

	child = start_child(argv + 1);

	/*
	 * The newline for a name is only printed once the next name is
	 * about to appear, instead of right away: this way, if the syscall
	 * itself writes to stdout (e.g. write(2)), its raw bytes land right
	 * after the name instead of on a line of their own.
	 */
	ptrace(PTRACE_GETREGS, child, NULL, &regs);
	printf("%s", get_syscall_name((long)regs.orig_rax));

	while (wait_for_syscall(child, &status))
	{
		if (entering)
		{
			ptrace(PTRACE_GETREGS, child, NULL, &regs);
			printf("\n%s", get_syscall_name((long)regs.orig_rax));
		}
		entering = !entering;
	}
	printf("\n");

	return (0);
}
