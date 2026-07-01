#include "strace.h"

/**
 * print_args - prints a syscall's arguments in hexadecimal
 * @regs: the traced process' registers at syscall-entry
 * @nargs: number of arguments to print (from the syscall table)
 * @variadic: 1 if a trailing "..." must be appended
 */
static void print_args(struct user_regs_struct regs, int nargs, int variadic)
{
	unsigned long long args[6];
	int i;

	args[0] = regs.rdi;
	args[1] = regs.rsi;
	args[2] = regs.rdx;
	args[3] = regs.r10;
	args[4] = regs.r8;
	args[5] = regs.r9;

	printf("(");
	for (i = 0; i < nargs; i++)
		printf("%s%#llx", i > 0 ? ", " : "", args[i]);
	if (variadic)
		printf("%s...", nargs > 0 ? ", " : "");
	printf(")");
}

/**
 * main - executes and traces a command, printing each syscall's name,
 * arguments and return value in hexadecimal
 * @argc: argument count
 * @argv: argument vector, argv[1] is the full path to the binary to run
 *
 * Return: 0 on success, 1 on error
 */
int main(int argc, char **argv)
{
	pid_t child;
	int status, entering = 1, pending = 0, nargs, variadic;
	struct user_regs_struct regs;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
		return (1);
	}

	child = start_child(argv + 1);

	/* the stop right after execve doubles as its own syscall-stop */
	ptrace(PTRACE_GETREGS, child, NULL, &regs);
	get_syscall_args(regs.orig_rax, &nargs, &variadic);
	printf("%s", get_syscall_name(regs.orig_rax));
	print_args(regs, nargs, variadic);
	printf(" = %#llx\n", regs.rax);

	while (wait_for_syscall(child, &status))
	{
		ptrace(PTRACE_GETREGS, child, NULL, &regs);
		if (entering)
		{
			get_syscall_args(regs.orig_rax, &nargs, &variadic);
			printf("%s", get_syscall_name(regs.orig_rax));
			print_args(regs, nargs, variadic);
			pending = 1;
		}
		else
		{
			printf(" = %#llx\n", regs.rax);
			pending = 0;
		}
		entering = !entering;
	}
	if (pending)
		printf(" = ?\n");

	return (0);
}
