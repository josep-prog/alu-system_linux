#ifndef HNM_H
#define HNM_H

#include "elf_common.h"
#include <ctype.h>

#define ST_BIND(info) ((info) >> 4)
#define ST_TYPE(info) ((info) & 0xf)

/* One resolved "name needs version X" entry taken from .dynsym */
typedef struct
{
	const char *name;
	const char *version;
} ver_entry_t;

int choose_symtab_section(const elf_t *elf);
sym_t *read_symtab(const elf_t *elf, int sh_index, size_t *count);

size_t build_version_map(const elf_t *elf, ver_entry_t **out);
const char *lookup_version(ver_entry_t *map, size_t count, const char *name);

char symbol_letter(const elf_t *elf, const sym_t *sym);
int skip_symbol(const sym_t *sym);
void print_symbol(const elf_t *elf, const sym_t *sym, const char *name,
		   ver_entry_t *map, size_t vcount);

int process_file(const char *filename, int show_header);

#endif /* HNM_H */
