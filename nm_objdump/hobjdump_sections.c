#include "hobjdump.h"

/**
 * width_for - number of hex digits used to display a section's addresses
 * @s: section to measure
 *
 * Return: the digit count needed for its last address, at least 4
 */
static int width_for(const sect_t *s)
{
	uint64_t last = s->sh_size ? s->sh_addr + s->sh_size - 1 : s->sh_addr;
	int digits = 1;

	while (last >>= 4)
		digits++;
	return (digits < 4 ? 4 : digits);
}

/**
 * print_hex_line - print one 16-byte row of a section hex dump
 * @addr: address of the first byte on this row
 * @width: digit width of the address column
 * @bytes: pointer to the row's bytes within the file buffer
 * @n: number of valid bytes in this row (1-16)
 */
static void print_hex_line(uint64_t addr, int width, const unsigned char *bytes,
			    int n)
{
	int i;

	printf(" %0*llx ", width, (unsigned long long)addr);
	for (i = 0; i < 16; i++)
	{
		if (i < n)
			printf("%02x", bytes[i]);
		else
			printf("  ");
		if (i % 4 == 3 && i != 15)
			printf(" ");
	}
	printf("  ");
	for (i = 0; i < n; i++)
		putchar(isprint(bytes[i]) ? bytes[i] : '.');
	for (; i < 16; i++)
		putchar(' ');
	printf("\n");
}

/**
 * skip_section - decide whether `objdump -s` would hide this section
 * @elf: parsed ELF file
 * @idx: section index to check
 * @strtab_of_symtab: index of .symtab's linked string table, or -1
 *
 * Return: 1 if the section must not be dumped, 0 otherwise
 */
static int skip_section(const elf_t *elf, int idx, int strtab_of_symtab)
{
	sect_t *s = &elf->sections[idx];

	if (idx == 0 || idx == elf->e_shstrndx || idx == strtab_of_symtab)
		return (1);
	if (s->sh_type == SHT_NOBITS_V || s->sh_size == 0)
		return (1);
	if (s->sh_type == 2 || s->sh_type == SHT_GROUP_V ||
	    s->sh_type == SHT_SYMTAB_SHNDX_V)
		return (1);
	if (elf->e_type == ET_REL_V &&
	    (s->sh_type == SHT_RELA_V || s->sh_type == SHT_REL_V))
		return (1);
	return (0);
}

/**
 * print_sections - dump the contents of every visible section
 * @elf: parsed ELF file
 */
void print_sections(const elf_t *elf)
{
	int symtab_idx = elf_find_section(elf, ".symtab");
	int strtab_idx = symtab_idx == -1 ? -1
			  : (int)elf->sections[symtab_idx].sh_link;
	uint16_t i;
	sect_t *s;
	uint64_t off;
	int width, n;

	for (i = 0; i < elf->e_shnum; i++)
	{
		if (skip_section(elf, i, strtab_idx))
			continue;
		s = &elf->sections[i];
		printf("Contents of section %s:\n", elf_shstr(elf, s->sh_name));
		width = width_for(s);
		for (off = 0; off < s->sh_size; off += 16)
		{
			n = (int)(s->sh_size - off < 16 ? s->sh_size - off : 16);
			print_hex_line(s->sh_addr + off, width,
					elf->data + s->sh_offset + off, n);
		}
	}
}
