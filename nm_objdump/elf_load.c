#include "elf_common.h"

/**
 * parse_ehdr - fill the ELF identification fields of @elf from its buffer
 * @elf: target structure, with data/size/is64/is_big_endian already set
 *
 * Return: 1 on success, 0 if the buffer is too small for the header
 */
static int parse_ehdr(elf_t *elf)
{
	unsigned char *b = elf->data;
	int be = elf->is_big_endian;
	size_t ehsize = elf->is64 ? 64 : 52;

	if (elf->size < ehsize)
		return (0);

	elf->e_type = (uint16_t)elf_read(b + 16, 2, be);
	elf->e_machine = (uint16_t)elf_read(b + 18, 2, be);
	elf->e_version = (uint32_t)elf_read(b + 20, 4, be);

	if (elf->is64)
	{
		elf->e_entry = elf_read(b + 24, 8, be);
		elf->e_phoff = elf_read(b + 32, 8, be);
		elf->e_shoff = elf_read(b + 40, 8, be);
		elf->e_flags = (uint32_t)elf_read(b + 48, 4, be);
		elf->e_phentsize = (uint16_t)elf_read(b + 54, 2, be);
		elf->e_phnum = (uint16_t)elf_read(b + 56, 2, be);
		elf->e_shentsize = (uint16_t)elf_read(b + 58, 2, be);
		elf->e_shnum = (uint16_t)elf_read(b + 60, 2, be);
		elf->e_shstrndx = (uint16_t)elf_read(b + 62, 2, be);
	}
	else
	{
		elf->e_entry = elf_read(b + 24, 4, be);
		elf->e_phoff = elf_read(b + 28, 4, be);
		elf->e_shoff = elf_read(b + 32, 4, be);
		elf->e_flags = (uint32_t)elf_read(b + 36, 4, be);
		elf->e_phentsize = (uint16_t)elf_read(b + 42, 2, be);
		elf->e_phnum = (uint16_t)elf_read(b + 44, 2, be);
		elf->e_shentsize = (uint16_t)elf_read(b + 46, 2, be);
		elf->e_shnum = (uint16_t)elf_read(b + 48, 2, be);
		elf->e_shstrndx = (uint16_t)elf_read(b + 50, 2, be);
	}
	return (1);
}

/**
 * parse_shdrs - allocate and fill the generic section header array
 * @elf: target structure with the header already parsed
 *
 * Return: 1 on success, 0 on allocation or bounds failure
 */
static int parse_shdrs(elf_t *elf)
{
	size_t entsize = elf->is64 ? 64 : 40;
	int be = elf->is_big_endian;
	unsigned char *base;
	sect_t *s;
	uint16_t i;

	elf->sections = NULL;
	if (elf->e_shnum == 0)
		return (1);
	if (elf->e_shoff + (uint64_t)elf->e_shnum * entsize > elf->size)
		return (0);

	elf->sections = malloc(sizeof(sect_t) * elf->e_shnum);
	if (!elf->sections)
		return (0);

	for (i = 0; i < elf->e_shnum; i++)
	{
		base = elf->data + elf->e_shoff + (size_t)i * entsize;
		s = &elf->sections[i];
		s->sh_name = (uint32_t)elf_read(base, 4, be);
		s->sh_type = (uint32_t)elf_read(base + 4, 4, be);
		if (elf->is64)
		{
			s->sh_flags = elf_read(base + 8, 8, be);
			s->sh_addr = elf_read(base + 16, 8, be);
			s->sh_offset = elf_read(base + 24, 8, be);
			s->sh_size = elf_read(base + 32, 8, be);
			s->sh_link = (uint32_t)elf_read(base + 40, 4, be);
			s->sh_info = (uint32_t)elf_read(base + 44, 4, be);
			s->sh_addralign = elf_read(base + 48, 8, be);
			s->sh_entsize = elf_read(base + 56, 8, be);
		}
		else
		{
			s->sh_flags = elf_read(base + 8, 4, be);
			s->sh_addr = elf_read(base + 12, 4, be);
			s->sh_offset = elf_read(base + 16, 4, be);
			s->sh_size = elf_read(base + 20, 4, be);
			s->sh_link = (uint32_t)elf_read(base + 24, 4, be);
			s->sh_info = (uint32_t)elf_read(base + 28, 4, be);
			s->sh_addralign = elf_read(base + 32, 4, be);
			s->sh_entsize = elf_read(base + 36, 4, be);
		}
	}
	return (1);
}

/**
 * elf_load - read a file from disk and parse it as an ELF object
 * @filename: path of the file to load
 * @elf: structure to fill in
 *
 * Return: 0 on success, 1 if the file cannot be opened/read,
 * 2 if the file is not a recognizable ELF object
 */
int elf_load(const char *filename, elf_t *elf)
{
	struct stat st;
	int fd;
	ssize_t n;

	memset(elf, 0, sizeof(*elf));

	fd = open(filename, O_RDONLY);
	if (fd < 0 || fstat(fd, &st) != 0)
	{
		if (fd >= 0)
			close(fd);
		return (1);
	}

	elf->size = (size_t)st.st_size;
	elf->data = malloc(elf->size ? elf->size : 1);
	if (!elf->data)
	{
		close(fd);
		return (1);
	}

	n = read(fd, elf->data, elf->size);
	close(fd);
	if (n < 0 || (size_t)n != elf->size)
	{
		free(elf->data);
		elf->data = NULL;
		return (1);
	}

	if (elf->size < 20 || elf->data[0] != 0x7f || elf->data[1] != 'E' ||
	    elf->data[2] != 'L' || elf->data[3] != 'F')
	{
		free(elf->data);
		elf->data = NULL;
		return (2);
	}

	elf->is64 = (elf->data[4] == 2);
	elf->is_big_endian = (elf->data[5] == 2);

	if (!parse_ehdr(elf) || !parse_shdrs(elf))
	{
		free(elf->data);
		elf->data = NULL;
		return (2);
	}

	return (0);
}
