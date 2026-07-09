# strace

A minimal reimplementation of `strace`, built directly on top of `ptrace(2)`.
Four incremental binaries trace a given command and its arguments, printing
more information about each intercepted syscall at every step.

## Usage

```
./strace_N command [args...]
```

`command` must be a full path to a binary (no `$PATH` lookup, e.g.
`/bin/echo`, not `echo`).

## Programs

### `strace_0` — syscall number

Prints the number of every syscall the traced process makes.

```
$ ./strace_0 /bin/echo Holberton
59
12
9
21
...
```

### `strace_1` — syscall name

Same as `strace_0`, but resolves the syscall number to its name using the
x86_64 syscall table (parsed from
`/usr/include/x86_64-linux-gnu/asm/unistd_64.h`, see `syscalls_x86_64.h`).

```
$ ./strace_1 /bin/echo Holberton
execve
brk
mmap
access
...
```

### `strace_2` — return value

Prints the syscall name and its return value in hexadecimal. Since the
process never returns from its very last syscall (`exit`/`exit_group`), its
return value is printed as `?`.

```
$ ./strace_2 /bin/echo Holberton
execve = 0
brk = 0x55bfb9386000
access = 0xfffffffffffffffe
...
exit_group = ?
```

### `strace_3` — raw parameters

Prints the syscall name, its arguments in hexadecimal (read from the
`rdi`, `rsi`, `rdx`, `r10`, `r8`, `r9` registers, following the x86_64
syscall calling convention), and its return value. Syscalls whose trailing
argument's type depends on an earlier argument (`ioctl`, `fcntl`, `open`,
`prctl`, ...) print their fixed arguments followed by `...`.

```
$ ./strace_3 /bin/echo Holberton
execve(0, 0, 0) = 0
brk(0) = 0x55c0697d3000
access(0x7f0da7020520, 0x4) = 0xfffffffffffffffe
openat(0xffffff9c, 0x7f0da701f2b8, 0x80000, 0, ...) = 0x3
...
exit_group(0) = ?
```

## Build

```
make strace_0   # or strace_1, strace_2, strace_3, or `make all`
```

## How it works

1. The parent `fork(2)`s. The child calls `PTRACE_TRACEME` then `execve(2)`s
   the target command; this raises a `SIGTRAP` in the child once the new
   image is loaded, which the parent catches with an initial `waitpid(2)`.
   That very stop already carries `execve`'s own return value in the `rax`
   register, so it is reported like any other syscall.
2. From then on, the parent repeatedly calls `ptrace(PTRACE_SYSCALL, ...)`
   to resume the child until its next syscall-stop, and `waitpid(2)` to
   wait for it. Each syscall triggers **two** stops: one on entry (right
   before the kernel executes it) and one on exit (right after, with the
   return value available). The tracer alternates between these two
   states to know when the return value in `rax` is valid.
3. At any stop, `ptrace(PTRACE_GETREGS, ...)` fills a `struct
   user_regs_struct` with the tracee's registers: `orig_rax` holds the
   syscall number, `rax` holds the return value (once past the syscall),
   and `rdi`/`rsi`/`rdx`/`r10`/`r8`/`r9` hold its first six arguments, in
   that order.

## Files

| File                  | Description                                         |
|-----------------------|------------------------------------------------------|
| `strace.h`             | Shared struct/prototype declarations                |
| `utils.c`              | `start_child`, `wait_for_syscall`                    |
| `get_syscall_name.c`   | Syscall number → name lookup                         |
| `syscalls_x86_64.h`    | Generated `{number, name}` table for x86_64          |
| `syscall_args.c`       | Syscall number → argument count / variadic lookup    |
| `main_0.c` .. `main_3.c` | Entry point for each `strace_N` binary             |
| `Makefile`             | `strace_0`, `strace_1`, `strace_2`, `strace_3`, `all`, `clean` |

## Notes

* Addresses, register values and even the exact set of syscalls a given
  binary makes are specific to the machine, kernel and glibc version used
  to run these programs — they will differ from the examples above.
* `PTRACE_SINGLESTEP` stops the tracee after **every single instruction**;
  `PTRACE_SYSCALL` stops it only on syscall entry/exit, which is what these
  programs use, since only syscall boundaries are of interest here.
