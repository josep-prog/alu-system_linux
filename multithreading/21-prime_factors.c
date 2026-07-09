#include <stdlib.h>
#include "multithreading.h"

/**
 * add_factor - Append a prime factor to a list, or strip every
 *              occurrence of it out of a number being factorized
 * @factors: List to append the factor(s) to
 * @n: Pointer to the number to strip d out of, or NULL to add once
 * @d: Factor value to store
 */
static void add_factor(list_t *factors, unsigned long int *n,
	unsigned long int d)
{
	unsigned long int *value;

	if (!n)
	{
		value = malloc(sizeof(*value));
		*value = d;
		list_add(factors, value);
		return;
	}
	while (*n % d == 0)
	{
		value = malloc(sizeof(*value));
		*value = d;
		list_add(factors, value);
		*n /= d;
	}
}

/**
 * modmath - Compute (a * b) % m, or (a ^ b) % m when is_pow is set,
 *           without overflowing 64 bit arithmetic
 * @a: Base
 * @b: Multiplier, or exponent when is_pow is set
 * @m: Modulus
 * @is_pow: 0 to multiply, 1 to exponentiate
 *
 * Return: The requested result, modulo m
 */
static unsigned long int modmath(unsigned long int a, unsigned long int b,
	unsigned long int m, int is_pow)
{
	unsigned long int result;

	if (!is_pow)
	{
		result = 0;
		a %= m;
		while (b > 0)
		{
			if (b & 1)
				result = (a >= m - result) ?
					result - (m - a) : result + a;
			a = (a >= m - a) ? a - (m - a) : a + a;
			b >>= 1;
		}
		return (result);
	}
	result = 1;
	a %= m;
	while (b > 0)
	{
		if (b & 1)
			result = modmath(result, a, m, 0);
		a = modmath(a, a, m, 0);
		b >>= 1;
	}
	return (result);
}

/**
 * is_prime - Deterministic Miller-Rabin primality test for 64 bit integers
 * @n: Number to test
 *
 * Return: 1 if n is prime, 0 otherwise
 */
static int is_prime(unsigned long int n)
{
	static unsigned long int const witnesses[] = {
		2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
	unsigned long int d, x, a;
	size_t i, j, r;

	if (n == 2)
		return (1);
	if (n < 2 || n % 2 == 0)
		return (0);
	d = n - 1;
	r = 0;
	while (d % 2 == 0)
	{
		d /= 2;
		r++;
	}
	for (i = 0; i < sizeof(witnesses) / sizeof(witnesses[0]); i++)
	{
		a = witnesses[i];
		if (a >= n)
			continue;
		x = modmath(a, d, n, 1);
		if (x == 1 || x == n - 1)
			continue;
		for (j = 0; j < r - 1; j++)
		{
			x = modmath(x, x, n, 0);
			if (x == n - 1)
				break;
		}
		if (x != n - 1)
			return (0);
	}
	return (1);
}

/**
 * pollard_rho - Find a non trivial factor of a composite number
 * @n: Composite number to factor
 *
 * Return: A factor of n (not necessarily prime)
 */
static unsigned long int pollard_rho(unsigned long int n)
{
	unsigned long int x, y, c, d, diff, a, b, t;

	if (n % 2 == 0)
		return (2);
	for (c = 1; c < n; c++)
	{
		x = 2;
		y = 2;
		d = 1;
		while (d == 1)
		{
			x = (modmath(x, x, n, 0) + c) % n;
			y = (modmath(y, y, n, 0) + c) % n;
			y = (modmath(y, y, n, 0) + c) % n;
			diff = (x > y) ? x - y : y - x;
			a = diff;
			b = n;
			while (b != 0)
			{
				t = b;
				b = a % b;
				a = t;
			}
			d = a;
		}
		if (d != n)
			return (d);
	}
	return (n);
}

/**
 * prime_factors - Factorize a number into a list of prime factors
 * @s: String representation of the number to factorize
 *
 * Return: Pointer to the list of prime factors, in ascending order
 */
list_t *prime_factors(char const *s)
{
	list_t *factors;
	unsigned long int big[64], stack[64], n, d, val;
	size_t bcount, top, i, j;

	factors = malloc(sizeof(*factors));
	list_init(factors);
	bcount = 0;
	n = strtoul(s, NULL, 10);
	for (d = 2; d < 1000000UL && d * d <= n; d += (d == 2 ? 1 : 2))
		add_factor(factors, &n, d);
	top = 0;
	if (n > 1)
		stack[top++] = n;
	while (top > 0)
	{
		val = stack[--top];
		if (is_prime(val))
			big[bcount++] = val;
		else
		{
			d = pollard_rho(val);
			stack[top++] = d;
			stack[top++] = val / d;
		}
	}
	for (i = 1; i < bcount; i++)
	{
		val = big[i];
		j = i;
		while (j > 0 && big[j - 1] > val)
		{
			big[j] = big[j - 1];
			j--;
		}
		big[j] = val;
	}
	for (i = 0; i < bcount; i++)
		add_factor(factors, NULL, big[i]);
	return (factors);
}
