#include "hnm.h"

/**
 * resolve_verneed - find the version name required for a given index
 * @elf: parsed ELF file
 * @verneed_idx: section index of .gnu.version_r
 * @target: version index to look for (already masked)
 *
 * Return: pointer to the version name string, or NULL if not found
 */
static const char *resolve_verneed(const elf_t *elf, int verneed_idx,
				    uint16_t target)
{
	sect_t *sec = &elf->sections[verneed_idx];
	sect_t *dynstr = &elf->sections[sec->sh_link];
	int be = elf->is_big_endian;
	unsigned char *vn, *vna;
	uint32_t vn_next, vna_next, vna_name;
	uint16_t vna_other;

	vn = elf->data + sec->sh_offset;
	while (1)
	{
		vna = vn + elf_read(vn + 8, 4, be);
		while (1)
		{
			vna_other = (uint16_t)elf_read(vna + 6, 2, be);
			vna_name = (uint32_t)elf_read(vna + 8, 4, be);
			if (vna_other == target)
				return ((const char *)(elf->data +
							dynstr->sh_offset + vna_name));
			vna_next = (uint32_t)elf_read(vna + 12, 4, be);
			if (vna_next == 0)
				break;
			vna += vna_next;
		}
		vn_next = (uint32_t)elf_read(vn + 12, 4, be);
		if (vn_next == 0)
			return (NULL);
		vn += vn_next;
	}
}

/**
 * resolve_verdef - find the version name defined for a given index
 * @elf: parsed ELF file
 * @verdef_idx: section index of .gnu.version_d
 * @target: version index to look for (already masked)
 *
 * Return: pointer to the version name string, or NULL if not found
 */
static const char *resolve_verdef(const elf_t *elf, int verdef_idx,
				   uint16_t target)
{
	sect_t *sec = &elf->sections[verdef_idx];
	sect_t *dynstr = &elf->sections[sec->sh_link];
	int be = elf->is_big_endian;
	unsigned char *vd, *vda;
	uint32_t vd_next, vda_name;
	uint16_t vd_ndx;

	vd = elf->data + sec->sh_offset;
	while (1)
	{
		vd_ndx = (uint16_t)elf_read(vd + 4, 2, be) & VERSYM_VERSION_MASK;
		if (vd_ndx == target)
		{
			vda = vd + elf_read(vd + 12, 4, be);
			vda_name = (uint32_t)elf_read(vda, 4, be);
			return ((const char *)(elf->data +
						dynstr->sh_offset + vda_name));
		}
		vd_next = (uint32_t)elf_read(vd + 16, 4, be);
		if (vd_next == 0)
			return (NULL);
		vd += vd_next;
	}
}

/**
 * build_version_map - associate each versioned dynamic symbol with its
 * required or defined version string, as GNU nm does for the "@@" suffix
 * @elf: parsed ELF file
 * @out: set to a malloc'd array of entries (NULL if none)
 *
 * Return: number of entries in *out
 */
size_t build_version_map(const elf_t *elf, ver_entry_t **out)
{
	int dynsym_idx = elf_find_section(elf, ".dynsym");
	int versym_idx = -1, verneed_idx = -1, verdef_idx = -1;
	sym_t *dynsyms;
	size_t count, used = 0, i;
	uint16_t versym, target;
	const char *dynstr_base, *version;
	uint16_t s;

	*out = NULL;
	if (dynsym_idx == -1)
		return (0);

	for (s = 0; s < elf->e_shnum; s++)
	{
		if (elf->sections[s].sh_type == SHT_GNU_VERSYM_V)
			versym_idx = s;
		else if (elf->sections[s].sh_type == SHT_GNU_VERNEED_V)
			verneed_idx = s;
		else if (elf->sections[s].sh_type == SHT_GNU_VERDEF_V)
			verdef_idx = s;
	}
	if (versym_idx == -1 || (verneed_idx == -1 && verdef_idx == -1))
		return (0);

	dynsyms = read_symtab(elf, dynsym_idx, &count);
	if (!dynsyms)
		return (0);
	dynstr_base = (const char *)(elf->data +
		elf->sections[elf->sections[dynsym_idx].sh_link].sh_offset);

	*out = malloc(sizeof(ver_entry_t) * count);
	if (!*out)
	{
		free(dynsyms);
		return (0);
	}

	for (i = 1; i < count; i++)
	{
		versym = (uint16_t)elf_read(elf->data +
			elf->sections[versym_idx].sh_offset + i * 2, 2,
			elf->is_big_endian);
		target = versym & VERSYM_VERSION_MASK;
		if (target < 2)
			continue;

		version = NULL;
		if (dynsyms[i].st_shndx == SHN_UNDEF_V && verneed_idx != -1)
			version = resolve_verneed(elf, verneed_idx, target);
		else if (dynsyms[i].st_shndx != SHN_UNDEF_V && verdef_idx != -1)
			version = resolve_verdef(elf, verdef_idx, target);
		if (!version)
			continue;

		(*out)[used].name = dynstr_base + dynsyms[i].st_name;
		(*out)[used].version = version;
		used++;
	}
	free(dynsyms);
	return (used);
}

/**
 * lookup_version - find the version string associated with a symbol name
 * @map: version map built by build_version_map
 * @count: number of entries in the map
 * @name: symbol name to look up
 *
 * Return: matching version string, or NULL if none
 */
const char *lookup_version(ver_entry_t *map, size_t count, const char *name)
{
	size_t i;

	for (i = 0; i < count; i++)
		if (strcmp(map[i].name, name) == 0)
			return (map[i].version);
	return (NULL);
}
