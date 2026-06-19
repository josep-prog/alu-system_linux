#include "hnm.h"

/**
 * symbol_letter - classify a symbol the way GNU nm picks its type letter
 * @elf: parsed ELF file
 * @sym: symbol entry to classify
 *
 * Return: the nm type character (case encodes local vs global binding)
 */
char symbol_letter(const elf_t *elf, const sym_t *sym)
{
	int bind = ST_BIND(sym->st_info);
	int type = ST_TYPE(sym->st_info);
	sect_t *sh;
	char base;

	if (sym->st_shndx == SHN_UNDEF_V)
		return (bind == STB_WEAK_V ? (type == STT_OBJECT_V ? 'v' : 'w') : 'U');
	if (sym->st_shndx == SHN_ABS_V)
		return (bind == STB_LOCAL_V ? 'a' : 'A');
	if (sym->st_shndx == SHN_COMMON_V)
		return (bind == STB_LOCAL_V ? 'c' : 'C');
	if (sym->st_shndx >= elf->e_shnum)
		return ('?');

	sh = &elf->sections[sym->st_shndx];
	if (type == STT_GNU_IFUNC_V)
		base = 'i';
	else if (bind == STB_WEAK_V)
		return (type == STT_OBJECT_V ? 'V' : 'W');
	else if (sh->sh_type == SHT_NOBITS_V)
		base = 'b';
	else if (sh->sh_flags & SHF_EXECINSTR_V)
		base = 't';
	else if (!(sh->sh_flags & SHF_ALLOC_V))
		base = 'n';
	else if (sh->sh_flags & SHF_WRITE_V)
		base = 'd';
	else
		base = 'r';

	return (bind == STB_LOCAL_V ? (char)tolower(base) : (char)toupper(base));
}

/**
 * skip_symbol - decide whether nm hides this symbol by default
 * @sym: symbol entry to check
 *
 * Return: 1 if the symbol must not be printed, 0 otherwise
 */
int skip_symbol(const sym_t *sym)
{
	int type = ST_TYPE(sym->st_info);

	return (type == STT_SECTION_V || type == STT_FILE_V);
}

/**
 * print_symbol - print one symbol line in `nm -p` format
 * @elf: parsed ELF file
 * @sym: symbol entry to print
 * @name: symbol name
 * @map: version map built by build_version_map
 * @vcount: number of entries in the version map
 */
void print_symbol(const elf_t *elf, const sym_t *sym, const char *name,
		   ver_entry_t *map, size_t vcount)
{
	int width = elf->is64 ? 16 : 8;
	char letter = symbol_letter(elf, sym);
	const char *version = lookup_version(map, vcount, name);
	char full[512];

	if (version)
		snprintf(full, sizeof(full), "%s@@%s", name, version);
	else
		snprintf(full, sizeof(full), "%s", name);

	if (sym->st_shndx == SHN_UNDEF_V)
		printf("%*s %c %s\n", width, "", letter, full);
	else
		printf("%0*lx %c %s\n", width,
		       (unsigned long)sym->st_value, letter, full);
}
