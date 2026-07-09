#include "strace.h"
#include "syscalls_x86_64.h"

/**
 * get_syscall_name - looks up the name of a syscall by its number
 * @number: the syscall number (orig_rax)
 *
 * Return: the syscall name, or "?" if unknown
 */
const char *get_syscall_name(long number)
{
	int i;

	for (i = 0; syscall_names[i].name != NULL; i++)
		if (syscall_names[i].number == number)
			return (syscall_names[i].name);
	return ("?");
}
