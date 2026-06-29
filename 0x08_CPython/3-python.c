#include <Python.h>
#include <stdio.h>

void print_python_bytes(PyObject *p)
{
	PyBytesObject *bytes;
	Py_ssize_t i, size, print_count;

	printf("[.] bytes object info\n");
	if (!PyBytes_Check(p))
	{
		printf("  [ERROR] Invalid Bytes Object\n");
		fflush(stdout);
		return;
	}

	bytes = (PyBytesObject *)p;
	size = ((PyVarObject *)p)->ob_size;

	printf("  size: %ld\n", size);
	printf("  trying string: %s\n", bytes->ob_sval);

	print_count = size + 1;
	if (print_count > 10)
		print_count = 10;

	printf("  first %ld bytes:", print_count);
	for (i = 0; i < print_count; i++)
		printf(" %02x", (unsigned char)bytes->ob_sval[i]);
	printf("\n");
	fflush(stdout);
}

void print_python_float(PyObject *p)
{
	PyObject *repr;

	printf("[.] float object info\n");
	if (!PyFloat_Check(p))
	{
		printf("  [ERROR] Invalid Float Object\n");
		fflush(stdout);
		return;
	}

	repr = PyObject_Repr(p);
	printf("  value: %s\n", (char *)((PyASCIIObject *)repr + 1));
	Py_DECREF(repr);
	fflush(stdout);
}

void print_python_list(PyObject *p)
{
	PyListObject *list;
	Py_ssize_t i, size, allocated;
	PyObject *item;

	printf("[*] Python list info\n");
	if (!PyList_Check(p))
	{
		printf("  [ERROR] Invalid List Object\n");
		fflush(stdout);
		return;
	}

	list = (PyListObject *)p;
	size = ((PyVarObject *)p)->ob_size;
	allocated = list->allocated;

	printf("[*] Size of the Python List = %ld\n", size);
	printf("[*] Allocated = %ld\n", allocated);

	for (i = 0; i < size; i++)
	{
		item = list->ob_item[i];
		printf("Element %ld: %s\n", i, ((PyObject *)item)->ob_type->tp_name);
		if (PyBytes_Check(item))
			print_python_bytes(item);
		else if (PyFloat_Check(item))
			print_python_float(item);
	}
	fflush(stdout);
}
