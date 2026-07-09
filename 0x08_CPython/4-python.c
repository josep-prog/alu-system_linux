#include <Python.h>
#include <stdio.h>
#include <stdint.h>

static void print_unicode_data(PyASCIIObject *ascii, Py_ssize_t length)
{
	int kind = ascii->state.kind;
	void *data = (PyCompactUnicodeObject *)(ascii) + 1;
	Py_ssize_t i;
	uint32_t cp;
	unsigned char buf[4];
	int n;

	for (i = 0; i < length; i++)
	{
		if (kind == 1)
			cp = ((uint8_t *)data)[i];
		else if (kind == 2)
			cp = ((uint16_t *)data)[i];
		else
			cp = ((uint32_t *)data)[i];

		if (cp < 0x80)
		{
			buf[0] = (unsigned char)cp;
			n = 1;
		}
		else if (cp < 0x800)
		{
			buf[0] = (unsigned char)(0xC0 | (cp >> 6));
			buf[1] = (unsigned char)(0x80 | (cp & 0x3F));
			n = 2;
		}
		else if (cp < 0x10000)
		{
			buf[0] = (unsigned char)(0xE0 | (cp >> 12));
			buf[1] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
			buf[2] = (unsigned char)(0x80 | (cp & 0x3F));
			n = 3;
		}
		else
		{
			buf[0] = (unsigned char)(0xF0 | (cp >> 18));
			buf[1] = (unsigned char)(0x80 | ((cp >> 12) & 0x3F));
			buf[2] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
			buf[3] = (unsigned char)(0x80 | (cp & 0x3F));
			n = 4;
		}
		fwrite(buf, 1, (size_t)n, stdout);
	}
}

void print_python_string(PyObject *p)
{
	PyASCIIObject *ascii;
	Py_ssize_t length;

	printf("[.] string object info\n");
	if (!PyUnicode_Check(p))
	{
		printf("  [ERROR] Invalid String Object\n");
		return;
	}

	ascii = (PyASCIIObject *)p;
	length = ascii->length;

	if (ascii->state.compact && ascii->state.ascii)
	{
		printf("  type: compact ascii\n");
		printf("  length: %ld\n", length);
		printf("  value: %s\n", (char *)(ascii + 1));
	}
	else
	{
		printf("  type: compact unicode object\n");
		printf("  length: %ld\n", length);
		printf("  value: ");
		print_unicode_data(ascii, length);
		printf("\n");
	}
}
