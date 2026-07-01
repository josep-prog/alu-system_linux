#include "strace.h"

/*
 * Argument counts below come from the x86_64 syscall prototypes (see
 * `man 2 <syscall>`). Only ioctl's trailing argument, whose type depends
 * on the request code, is marked variadic (printed as "..."); every
 * other syscall prints a fixed number of hexadecimal arguments.
 */
static const syscall_args_t syscall_args[] = {
	{0, 3, 0},   /* read(fd, buf, count) */
	{1, 3, 0},   /* write(fd, buf, count) */
	{2, 2, 0},   /* open(path, flags) */
	{3, 1, 0},   /* close(fd) */
	{4, 2, 0},   /* stat(path, buf) */
	{5, 2, 0},   /* fstat(fd, buf) */
	{6, 2, 0},   /* lstat(path, buf) */
	{7, 3, 0},   /* poll(fds, nfds, timeout) */
	{8, 3, 0},   /* lseek(fd, offset, whence) */
	{9, 6, 0},   /* mmap(addr, len, prot, flags, fd, off) */
	{10, 3, 0},  /* mprotect(addr, len, prot) */
	{11, 2, 0},  /* munmap(addr, len) */
	{12, 1, 0},  /* brk(addr) */
	{13, 3, 0},  /* rt_sigaction(sig, act, oldact) */
	{14, 3, 0},  /* rt_sigprocmask(how, set, oldset) */
	{15, 0, 0},  /* rt_sigreturn */
	{16, 2, 1},  /* ioctl(fd, cmd, ...arg) */
	{17, 4, 0},  /* pread64 */
	{18, 4, 0},  /* pwrite64 */
	{19, 3, 0},  /* readv */
	{20, 3, 0},  /* writev */
	{21, 2, 0},  /* access(path, mode) */
	{22, 1, 0},  /* pipe(fds) */
	{23, 5, 0},  /* select */
	{24, 0, 0},  /* sched_yield */
	{25, 5, 0},  /* mremap */
	{26, 3, 0},  /* msync */
	{27, 3, 0},  /* mincore */
	{28, 3, 0},  /* madvise */
	{29, 3, 0},  /* shmget */
	{30, 3, 0},  /* shmat */
	{31, 3, 0},  /* shmctl */
	{32, 1, 0},  /* dup(fd) */
	{33, 2, 0},  /* dup2(oldfd, newfd) */
	{34, 0, 0},  /* pause */
	{35, 2, 0},  /* nanosleep */
	{36, 2, 0},  /* getitimer */
	{37, 1, 0},  /* alarm */
	{38, 3, 0},  /* setitimer */
	{39, 0, 0},  /* getpid */
	{40, 4, 0},  /* sendfile */
	{41, 3, 0},  /* socket */
	{42, 3, 0},  /* connect */
	{43, 3, 0},  /* accept */
	{44, 6, 0},  /* sendto */
	{45, 6, 0},  /* recvfrom */
	{46, 3, 0},  /* sendmsg */
	{47, 3, 0},  /* recvmsg */
	{48, 2, 0},  /* shutdown */
	{49, 3, 0},  /* bind */
	{50, 2, 0},  /* listen */
	{51, 3, 0},  /* getsockname */
	{52, 3, 0},  /* getpeername */
	{53, 4, 0},  /* socketpair */
	{54, 5, 0},  /* setsockopt */
	{55, 5, 0},  /* getsockopt */
	{56, 5, 0},  /* clone */
	{57, 0, 0},  /* fork */
	{58, 0, 0},  /* vfork */
	{59, 3, 0},  /* execve(path, argv, envp) */
	{60, 1, 0},  /* exit(status) */
	{61, 4, 0},  /* wait4 */
	{62, 2, 0},  /* kill */
	{63, 1, 0},  /* uname */
	{72, 2, 0},  /* fcntl(fd, cmd) */
	{78, 3, 0},  /* getdents */
	{79, 2, 0},  /* getcwd */
	{80, 1, 0},  /* chdir */
	{81, 1, 0},  /* fchdir */
	{82, 2, 0},  /* rename */
	{83, 2, 0},  /* mkdir */
	{84, 1, 0},  /* rmdir */
	{85, 2, 0},  /* creat */
	{86, 2, 0},  /* link */
	{87, 1, 0},  /* unlink */
	{88, 2, 0},  /* symlink */
	{89, 3, 0},  /* readlink */
	{90, 2, 0},  /* chmod */
	{91, 2, 0},  /* fchmod */
	{92, 3, 0},  /* chown */
	{93, 3, 0},  /* fchown */
	{94, 3, 0},  /* lchown */
	{95, 1, 0},  /* umask */
	{96, 2, 0},  /* gettimeofday */
	{97, 2, 0},  /* getrlimit */
	{102, 0, 0}, /* getuid */
	{104, 0, 0}, /* getgid */
	{105, 1, 0}, /* setuid */
	{106, 1, 0}, /* setgid */
	{107, 0, 0}, /* geteuid */
	{108, 0, 0}, /* getegid */
	{110, 0, 0}, /* getppid */
	{157, 5, 0}, /* prctl(option, arg2, arg3, arg4, arg5) */
	{158, 2, 0}, /* arch_prctl(code, addr) */
	{186, 0, 0}, /* gettid */
	{202, 6, 0}, /* futex */
	{217, 3, 0}, /* getdents64 */
	{218, 1, 0}, /* set_tid_address */
	{228, 2, 0}, /* clock_gettime */
	{231, 1, 0}, /* exit_group(status) */
	{257, 3, 0}, /* openat(dirfd, path, flags) */
	{262, 4, 0}, /* newfstatat */
	{270, 3, 0}, /* pselect6 */
	{273, 2, 0}, /* set_robust_list */
	{280, 4, 0}, /* utimensat */
	{291, 5, 0}, /* epoll_create1 */
	{293, 2, 0}, /* pipe2 */
	{318, 3, 0}, /* getrandom */
	{-1, 0, 0}
};

/**
 * get_syscall_args - looks up the argument count of a syscall
 * @number: the syscall number (orig_rax)
 * @nargs: where the number of hexadecimal arguments to print is stored
 * @variadic: where the variadic flag is stored
 */
void get_syscall_args(long number, int *nargs, int *variadic)
{
	int i;

	for (i = 0; syscall_args[i].number != -1; i++)
	{
		if (syscall_args[i].number == number)
		{
			*nargs = syscall_args[i].nargs;
			*variadic = syscall_args[i].variadic;
			return;
		}
	}
	*nargs = 3;
	*variadic = 0;
}
