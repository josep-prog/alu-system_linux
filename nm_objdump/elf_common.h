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
#define SHT_INIT_ARRAY_V 14
#define SHT_FINI_ARRAY_V 15
#define SHT_PREINIT_ARRAY_V 16
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

/**
 * struct sect_t - generic in-memory view of one ELF section header,
 * widened to 64 bits
 * @sh_name: byte offset of the section name in .shstrtab
 * @sh_type: section type
 * @sh_flags: section attribute flags
 * @sh_addr: virtual address of the section in memory
 * @sh_offset: byte offset of the section in the file
 * @sh_size: size of the section in bytes
 * @sh_link: section header table index link
 * @sh_info: extra section information
 * @sh_addralign: required address alignment
 * @sh_entsize: size of each entry, for sections holding fixed-size entries
 */
typedef struct sect_t
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

/**
 * struct sym_t - generic in-memory view of one ELF symbol table entry,
 * widened to 64 bits
 * @st_name: byte offset of the symbol name in its string table
 * @st_info: symbol type and binding
 * @st_other: symbol visibility
 * @st_shndx: index of the section the symbol is defined in
 * @st_value: value or address of the symbol
 * @st_size: size of the symbol's associated data
 */
typedef struct sym_t
{
	uint32_t st_name;
	uint8_t st_info;
	uint8_t st_other;
	uint16_t st_shndx;
	uint64_t st_value;
	uint64_t st_size;
} sym_t;

/**
 * struct elf_t - parsed ELF identity and section/symbol tables shared by
 * hnm and hobjdump
 * @data: raw file content loaded in memory
 * @size: size of @data in bytes
 * @is64: 1 if this is a 64-bit ELF file, 0 if 32-bit
 * @is_big_endian: 1 if the file is big-endian, 0 if little-endian
 * @e_type: object file type
 * @e_machine: target architecture
 * @e_version: object file version
 * @e_entry: virtual address of the entry point
 * @e_phoff: byte offset of the program header table
 * @e_shoff: byte offset of the section header table
 * @e_flags: processor-specific flags
 * @e_phentsize: size of one program header table entry
 * @e_phnum: number of program header table entries
 * @e_shentsize: size of one section header table entry
 * @e_shnum: number of section header table entries
 * @e_shstrndx: section header table index of the section name string table
 * @sections: parsed array of @e_shnum generic section headers
 */
typedef struct elf_t
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
