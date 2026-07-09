#include "hobjdump.h"

/**
 * arch_name - resolve the architecture string and bfd target name
 * @elf: parsed ELF file
 * @fmt_out: set to the matching "elfNN-..." target name
 *
 * Return: the human readable architecture name used by `objdump -f`
 */
static const char *arch_name(const elf_t *elf, const char **fmt_out)
{
	int be = elf->is_big_endian, w = elf->is64;

	switch (elf->e_machine)
	{
	case 3:
		*fmt_out = "elf32-i386";
		return ("i386");
	case 62:
		*fmt_out = "elf64-x86-64";
		return ("i386:x86-64");
	case 40:
		*fmt_out = be ? "elf32-bigarm" : "elf32-littlearm";
		return ("arm");
	case 183:
		*fmt_out = be ? "elf64-bigaarch64" : "elf64-littleaarch64";
		return ("aarch64");
	case 8:
		*fmt_out = w ? (be ? "elf64-tradbigmips" : "elf64-tradlittlemips")
			     : (be ? "elf32-tradbigmips" : "elf32-tradlittlemips");
		return ("mips");
	case 20:
		*fmt_out = be ? "elf32-powerpc" : "elf32-powerpcle";
		return ("powerpc");
	case 21:
		*fmt_out = be ? "elf64-powerpc" : "elf64-powerpcle";
		return ("powerpc:common64");
	case 2:
		*fmt_out = "elf32-sparc";
		return ("sparc");
	case 43:
		*fmt_out = "elf64-sparc";
		return ("sparc:v9");
	case 243:
		*fmt_out = w ? "elf64-littleriscv" : "elf32-littleriscv";
		return ("riscv");
	default:
		*fmt_out = w ? (be ? "elf64-big" : "elf64-little")
			     : (be ? "elf32-big" : "elf32-little");
		return ("unknown");
	}
}

/**
 * compute_flags - rebuild the synthetic bfd file flags `objdump -f` shows
 * @elf: parsed ELF file
 *
 * Return: bitmask combining HAS_RELOC, EXEC_P, HAS_SYMS, DYNAMIC, D_PAGED
 */
static uint32_t compute_flags(const elf_t *elf)
{
	uint32_t flags = 0;
	int has_syms = elf_find_section(elf, ".symtab") != -1 ||
			elf_find_section(elf, ".dynsym") != -1;

	if (elf->e_type == ET_REL_V)
		flags |= 0x00000001;
	if (elf->e_type == ET_EXEC_V)
		flags |= 0x00000002;
	if (has_syms)
		flags |= 0x00000010;
	if (elf->e_type == ET_DYN_V)
		flags |= 0x00000040;
	if ((elf->e_type == ET_EXEC_V || elf->e_type == ET_DYN_V) &&
	    elf->e_phnum > 0)
		flags |= 0x00000100;
	return (flags);
}

/**
 * print_flag_names - print the comma separated names of the set flag bits
 * @flags: bitmask produced by compute_flags
 */
static void print_flag_names(uint32_t flags)
{
	static const struct flag_name
	{
		uint32_t bit;
		const char *name;
	} table[] = {
		{0x00000001, "HAS_RELOC"}, {0x00000002, "EXEC_P"},
		{0x00000010, "HAS_SYMS"}, {0x00000040, "DYNAMIC"},
		{0x00000100, "D_PAGED"},
	};
	size_t i;
	int first = 1;

	for (i = 0; i < sizeof(table) / sizeof(table[0]); i++)
	{
		if (!(flags & table[i].bit))
			continue;
		printf("%s%s", first ? "" : ", ", table[i].name);
		first = 0;
	}
	printf("\n");
}

/**
 * print_header - print the `objdump -f` style header for one file
 * @elf: parsed ELF file
 * @filename: path that was loaded
 */
void print_header(const elf_t *elf, const char *filename)
{
	const char *fmt;
	const char *arch = arch_name(elf, &fmt);
	uint32_t flags = compute_flags(elf);

	printf("\n%s:     file format %s\n", filename, fmt);
	printf("architecture: %s, flags 0x%08x:\n", arch, flags);
	print_flag_names(flags);
	printf("start address 0x%0*" PRIx64 "\n\n",
	       elf->is64 ? 16 : 8, elf->e_entry);
}
