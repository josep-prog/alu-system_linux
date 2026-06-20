#include "elf_common.h"

/**
 * elf_read - read an integer field of a given width and endianness
 * @p: pointer to the first byte of the field
 * @size: width in bytes (1, 2, 4 or 8)
 * @big_endian: 1 if the field is stored most-significant-byte first
 *
 * Return: the field value widened to uint64_t
 */
uint64_t elf_read(const unsigned char *p, int size, int big_endian)
{
	uint64_t value = 0;
	int i;

	for (i = 0; i < size; i++)
	{
		int shift = big_endian ? (size - 1 - i) * 8 : i * 8;

		value |= ((uint64_t)p[i]) << shift;
	}
	return (value);
}

/**
 * elf_free - release the memory held by a parsed ELF structure
 * @elf: structure to clean up
 */
void elf_free(elf_t *elf)
{
	free(elf->sections);
	free(elf->data);
	elf->sections = NULL;
	elf->data = NULL;
}

/**
 * elf_shstr - fetch a string from the section header string table
 * @elf: parsed ELF file
 * @name_off: byte offset of the string within .shstrtab
 *
 * Return: pointer to the NUL-terminated string, or "" if unavailable
 */
const char *elf_shstr(const elf_t *elf, uint32_t name_off)
{
	sect_t *strtab;

	if (elf->e_shstrndx >= elf->e_shnum || !elf->sections)
		return ("");
	strtab = &elf->sections[elf->e_shstrndx];
	if (strtab->sh_offset + name_off >= elf->size)
		return ("");
	return ((const char *)(elf->data + strtab->sh_offset + name_off));
}

/**
 * elf_find_section - locate a section by name
 * @elf: parsed ELF file
 * @name: section name to search for
 *
 * Return: section index, or -1 if not found
 */
int elf_find_section(const elf_t *elf, const char *name)
{
	uint16_t i;

	for (i = 0; i < elf->e_shnum; i++)
		if (strcmp(elf_shstr(elf, elf->sections[i].sh_name), name) == 0)
			return (i);
	return (-1);
}

/**
 * elf_find_section_by_type - locate the first section of a given sh_type
 * @elf: parsed ELF file
 * @type: section type to search for
 *
 * Return: section index, or -1 if not found
 */
int elf_find_section_by_type(const elf_t *elf, uint32_t type)
{
	uint16_t i;

	for (i = 0; i < elf->e_shnum; i++)
		if (elf->sections[i].sh_type == type)
			return (i);
	return (-1);
}
