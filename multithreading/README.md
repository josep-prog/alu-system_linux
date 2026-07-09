# Multithreading

Low-level C project exploring POSIX threads: creating and joining threads,
avoiding race conditions with mutexes, and using a thread pool to speed up
CPU-bound work such as Gaussian blur and prime factorization.

## Learning Objectives

* What is a thread
* The differences between a thread and a process
* The difference between concurrency and parallelism
* How to create a thread
* How to properly exit a thread
* How to handle mutual exclusion
* What is a deadlock
* What is a race condition

## Files

| File | Description |
| --- | --- |
| `multithreading.h` | Project header: data structures and function prototypes |
| `list.h` / `list.c` | Generic doubly linked list used by tasks 5 and 6 |
| `0-thread_entry.c` | Thread entry point that prints a string |
| `1-tprintf.c` | Thread-safe-looking logger (no mutex, race condition visible) |
| `10-blur_portion.c` | Blurs a rectangular portion of an image |
| `11-blur_image.c` | Blurs an entire image by splitting it across up to 16 threads |
| `20-tprintf.c` | Logger v2, protected by a mutex initialized/destroyed via GCC constructor/destructor attributes |
| `21-prime_factors.c` | Factorizes a number into its prime factors |
| `22-prime_factors.c` | Thread pool (`create_task`, `destroy_task`, `exec_tasks`) executing a shared task list exactly once per task |

## Compilation

Each file is compiled with:

```sh
gcc -Wall -Werror -Wextra -pedantic -g3 <files> -o <output> -pthread
```

(`-pthread` is only required for files that use POSIX threads; task 5 does
not need it.)

## Testing images

`images/` contains a sample PBM (binary PPM `P6`) image and a Gaussian
kernel used to manually exercise the blur tasks:

```sh
./11-blur_image images/car.pbm images/kernel_0.knl
```
