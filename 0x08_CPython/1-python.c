#include <Python.h>
#include <stdio.h>

void print_python_list(PyObject *p)
{
	PyListObject *list;
	Py_ssize_t i, size, allocated;
	PyObject *item;

	list = (PyListObject *)p;
	size = ((PyVarObject *)p)->ob_size;
	allocated = list->allocated;

	printf("[*] Python list info\n");
	printf("[*] Size of the Python List = %ld\n", size);
	printf("[*] Allocated = %ld\n", allocated);

	for (i = 0; i < size; i++)
	{
		item = list->ob_item[i];
		printf("Element %ld: %s\n", i, ((PyObject *)item)->ob_type->tp_name);
	}
}
