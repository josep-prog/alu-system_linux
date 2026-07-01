#include "strace.h"

/**
 * main - executes and traces a command, printing each syscall's name
 * and return value in hexadecimal
 * @argc: argument count
 * @argv: argument vector, argv[1] is the full path to the binary to run
 *
 * Return: 0 on success, 1 on error
 */
int main(int argc, char **argv)
{
	pid_t child;
	int status, entering = 1, pending = 0;
	struct user_regs_struct regs;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
		return (1);
	}

	child = start_child(argv + 1);

	/* the stop right after execve doubles as its own syscall-stop */
	ptrace(PTRACE_GETREGS, child, NULL, &regs);
	printf("%s = %#lx\n", get_syscall_name((long)regs.orig_rax),
	       (long)regs.rax);

	while (wait_for_syscall(child, &status))
	{
		ptrace(PTRACE_GETREGS, child, NULL, &regs);
		if (entering)
		{
			printf("%s", get_syscall_name((long)regs.orig_rax));
			pending = 1;
		}
		else
		{
			printf(" = %#lx\n", (long)regs.rax);
			pending = 0;
		}
		entering = !entering;
	}
	if (pending)
		printf(" = ?\n");

	return (0);
}
