#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include "multithreading.h"

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

	printf("[%lu] ", (unsigned long)pthread_self());

	va_start(args, format);
	ret = vprintf(format, args);
	va_end(args);

	return (ret);
}
