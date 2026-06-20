#ifndef ELF_COMMON_H
#define ELF_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define SHN_UNDEF_V 0
#define SHN_ABS_V 0xfff1
#define SHN_COMMON_V 0xfff2
#define SHN_LORESERVE_V 0xff00

#define SHT_NOBITS_V 8
#define SHT_DYNSYM_V 11
#define SHT_GNU_VERSYM_V 0x6fffffff
#define SHT_GNU_VERNEED_V 0x6ffffffe
#define SHT_GNU_VERDEF_V 0x6ffffffd

#define SHF_WRITE_V 0x1
#define SHF_ALLOC_V 0x2
#define SHF_EXECINSTR_V 0x4

#define STB_LOCAL_V 0
#define STB_GLOBAL_V 1
#define STB_WEAK_V 2
#define STB_GNU_UNIQUE_V 10

#define STT_NOTYPE_V 0
#define STT_OBJECT_V 1
#define STT_FUNC_V 2
#define STT_SECTION_V 3
#define STT_FILE_V 4
#define STT_GNU_IFUNC_V 10

#define VERSYM_VERSION_MASK 0x7fff

/* Generic in-memory view of one ELF section header, widened to 64 bits */
typedef struct
{
	uint32_t sh_name;
	uint32_t sh_type;
	uint64_t sh_flags;
	uint64_t sh_addr;
	uint64_t sh_offset;
	uint64_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint64_t sh_addralign;
	uint64_t sh_entsize;
} sect_t;

/* Generic in-memory view of one ELF symbol table entry, widened to 64 bits */
typedef struct
{
	uint32_t st_name;
	uint8_t st_info;
	uint8_t st_other;
	uint16_t st_shndx;
	uint64_t st_value;
	uint64_t st_size;
} sym_t;

/* Parsed ELF identity and section/symbol tables shared by hnm and hobjdump */
typedef struct
{
	unsigned char *data;
	size_t size;
	int is64;
	int is_big_endian;
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint64_t e_entry;
	uint64_t e_phoff;
	uint64_t e_shoff;
	uint32_t e_flags;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
	sect_t *sections;
} elf_t;

uint64_t elf_read(const unsigned char *p, int size, int big_endian);
int elf_load(const char *filename, elf_t *elf);
void elf_free(elf_t *elf);
const char *elf_shstr(const elf_t *elf, uint32_t name_off);
int elf_find_section(const elf_t *elf, const char *name);
int elf_find_section_by_type(const elf_t *elf, uint32_t type);

#endif /* ELF_COMMON_H */
