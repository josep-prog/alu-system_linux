#include "hnm.h"

/**
 * choose_symtab_section - locate the static symbol table, like plain nm does
 * @elf: parsed ELF file
 *
 * Return: section index of .symtab, or -1 if the file has none
 */
int choose_symtab_section(const elf_t *elf)
{
	return (elf_find_section(elf, ".symtab"));
}

/**
 * read_symtab - load every entry of a symbol table section
 * @elf: parsed ELF file
 * @sh_index: section index of the symbol table
 * @count: set to the number of entries read
 *
 * Return: malloc'd array of generic symbol entries, or NULL on failure
 */
sym_t *read_symtab(const elf_t *elf, int sh_index, size_t *count)
{
	sect_t *sec = &elf->sections[sh_index];
	size_t entsize = elf->is64 ? 24 : 16;
	int be = elf->is_big_endian;
	unsigned char *base;
	sym_t *syms;
	size_t i;

	*count = sec->sh_entsize ? sec->sh_size / sec->sh_entsize
				  : sec->sh_size / entsize;
	if (*count == 0)
		return (NULL);

	syms = malloc(sizeof(sym_t) * (*count));
	if (!syms)
		return (NULL);

	for (i = 0; i < *count; i++)
	{
		base = elf->data + sec->sh_offset + i * entsize;
		if (elf->is64)
		{
			syms[i].st_name = (uint32_t)elf_read(base, 4, be);
			syms[i].st_info = base[4];
			syms[i].st_other = base[5];
			syms[i].st_shndx = (uint16_t)elf_read(base + 6, 2, be);
			syms[i].st_value = elf_read(base + 8, 8, be);
			syms[i].st_size = elf_read(base + 16, 8, be);
		}
		else
		{
			syms[i].st_name = (uint32_t)elf_read(base, 4, be);
			syms[i].st_value = elf_read(base + 4, 4, be);
			syms[i].st_size = elf_read(base + 8, 4, be);
			syms[i].st_info = base[12];
			syms[i].st_other = base[13];
			syms[i].st_shndx = (uint16_t)elf_read(base + 14, 2, be);
		}
	}
	return (syms);
}
