#include <Python.h>
#include <longintrepr.h>
#include <stdio.h>

void print_python_int(PyObject *p)
{
	PyLongObject *lng;
	Py_ssize_t ndigits, i;
	unsigned long value;
	int negative;

	if (!PyLong_Check(p))
	{
		printf("Invalid Int Object\n");
		return;
	}

	lng = (PyLongObject *)p;
	ndigits = ((PyVarObject *)p)->ob_size;
	negative = 0;

	if (ndigits < 0)
	{
		negative = 1;
		ndigits = -ndigits;
	}

	if (ndigits == 0)
	{
		printf("0\n");
		return;
	}

	value = 0;
	for (i = ndigits - 1; i >= 0; i--)
	{
		unsigned long digit = lng->ob_digit[i];

		if (value > (unsigned long)-1 >> PyLong_SHIFT)
		{
			printf("C unsigned long int overflow\n");
			return;
		}
		value = (value << PyLong_SHIFT) | digit;
	}

	if (negative)
		printf("-%lu\n", value);
	else
		printf("%lu\n", value);
}
