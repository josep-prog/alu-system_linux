#ifndef HOBJDUMP_H
#define HOBJDUMP_H

#include "elf_common.h"
#include <ctype.h>
#include <inttypes.h>

#define ET_REL_V 1
#define ET_EXEC_V 2
#define ET_DYN_V 3

#define SHT_RELA_V 4
#define SHT_REL_V 9
#define SHT_GROUP_V 17
#define SHT_SYMTAB_SHNDX_V 18

void process_file(const char *filename);
void print_header(const elf_t *elf, const char *filename);
void print_sections(const elf_t *elf);

#endif /* HOBJDUMP_H */
