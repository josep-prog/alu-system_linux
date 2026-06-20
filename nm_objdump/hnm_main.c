#include "hnm.h"

/**
 * print_symbols - print every relevant symbol of a parsed ELF's symtab
 * @elf: parsed ELF file
 * @symtab_idx: section index of the symbol table to print
 */
static void print_symbols(const elf_t *elf, int symtab_idx)
{
	sect_t *strtab = &elf->sections[elf->sections[symtab_idx].sh_link];
	sym_t *syms;
	ver_entry_t *vmap;
	size_t count, vcount, i;
	const char *name;

	syms = read_symtab(elf, symtab_idx, &count);
	vcount = build_version_map(elf, &vmap);

	for (i = 0; syms && i < count; i++)
	{
		if (skip_symbol(&syms[i]))
			continue;
		name = (const char *)(elf->data + strtab->sh_offset +
				       syms[i].st_name);
		if (name[0] == '\0')
			continue;
		print_symbol(elf, &syms[i], name, vmap, vcount);
	}

	free(syms);
	free(vmap);
}

/**
 * process_file - load one object file and print its symbols like `nm -p`
 * @filename: path of the file to process
 * @show_header: 1 if a "filename:" header line must be printed first
 *
 * Return: 0 on success, 1 if an error was reported for this file
 */
int process_file(const char *filename, int show_header)
{
	elf_t elf;
	int rc = elf_load(filename, &elf);
	int symtab_idx;

	if (rc == 1)
	{
		fprintf(stderr, "hnm: '%s': No such file\n", filename);
		return (1);
	}
	if (rc == 2)
	{
		fprintf(stderr, "hnm: %s: file format not recognized\n",
			filename);
		return (1);
	}

	if (show_header)
		printf("\n%s:\n", filename);

	symtab_idx = choose_symtab_section(&elf);
	if (symtab_idx == -1)
	{
		fprintf(stderr, "hnm: %s: no symbols\n", filename);
		elf_free(&elf);
		return (1);
	}

	print_symbols(&elf, symtab_idx);
	elf_free(&elf);
	return (0);
}

/**
 * main - entry point for hnm
 * @argc: argument count
 * @argv: argument vector
 *
 * Return: 0 if every file was processed successfully, 1 otherwise
 */
int main(int argc, char **argv)
{
	int status = 0, i;

	if (argc == 1)
		return (process_file("a.out", 0));

	for (i = 1; i < argc; i++)
		if (process_file(argv[i], argc > 2))
			status = 1;

	return (status);
}
