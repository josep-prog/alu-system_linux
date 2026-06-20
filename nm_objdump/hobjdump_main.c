#include "hobjdump.h"

static const char *prog_name = "hobjdump";

/**
 * process_file - load one object file and dump it like `objdump -sf`
 * @filename: path of the file to process
 */
void process_file(const char *filename)
{
	elf_t elf;
	int rc = elf_load(filename, &elf);

	if (rc == 1)
	{
		fprintf(stderr, "%s: '%s': No such file\n", prog_name, filename);
		return;
	}
	if (rc == 2)
	{
		fprintf(stderr, "%s: %s: file format not recognized\n",
			prog_name, filename);
		return;
	}

	print_header(&elf, filename);
	print_sections(&elf);
	elf_free(&elf);
}

/**
 * main - entry point for hobjdump
 * @argc: argument count
 * @argv: argument vector
 *
 * Return: 0 always, matching objdump's relaxed exit behavior
 */
int main(int argc, char **argv)
{
	int i;

	prog_name = argv[0];
	for (i = 1; i < argc; i++)
		process_file(argv[i]);

	return (0);
}
