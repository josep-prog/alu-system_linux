#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include "multithreading.h"

static pthread_mutex_t log_lock;

/**
 * init_log_lock - Constructor, initializes the logger mutex
 */
__attribute__((constructor))
static void init_log_lock(void)
{
	pthread_mutex_init(&log_lock, NULL);
}

/**
 * destroy_log_lock - Destructor, destroys the logger mutex
 */
__attribute__((destructor))
static void destroy_log_lock(void)
{
	pthread_mutex_destroy(&log_lock);
}

/**
 * tprintf - Print a formatted string preceded by the calling thread ID
 * @format: Format string, followed by its arguments
 *
 * Return: Number of characters printed, or a negative value on failure
 */
int tprintf(char const *format, ...)
{
	va_list args;
	int ret;

	pthread_mutex_lock(&log_lock);

	printf("[%lu] ", (unsigned long)pthread_self());

	va_start(args, format);
	ret = vprintf(format, args);
	va_end(args);

	pthread_mutex_unlock(&log_lock);

	return (ret);
}
