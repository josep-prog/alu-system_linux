#include <stdio.h>
#include <pthread.h>
#include "multithreading.h"

/**
 * thread_entry - Entry point for a new thread, prints out a string
 * @arg: Address of the string to print
 *
 * Return: NULL
 */
void *thread_entry(void *arg)
{
	printf("%s\n", (char *)arg);
	pthread_exit(NULL);
}
