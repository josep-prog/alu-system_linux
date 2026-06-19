# nm_objdump

Reimplementations of GNU `nm -p` and `objdump -sf`, written from scratch by
parsing the ELF format directly (no `libelf`/`libbfd`, no `elf.h` structs).

## Programs

### `hnm`

```
./hnm [objfile ...]
```

Prints the symbol table of one or more ELF files in the same format as
`nm -p`: one line per symbol, in symbol-table order (unsorted), as
`<value or blanks> <type letter> <name>`.

* Reads `.symtab` (falls back to reporting "no symbols" if absent, exactly
  like plain `nm`).
* Classifies each symbol's type letter the way `bfd_decode_symclass` does:
  section flags (`SHF_EXECINSTR`/`SHF_WRITE`/`SHF_ALLOC`), `SHN_ABS`,
  `SHN_COMMON`, `SHN_UNDEF`, weak binding, and `STT_GNU_IFUNC`, with case
  driven by local/global binding.
* Resolves the `@@VERSION` suffix on versioned dynamic symbols by cross
  referencing `.dynsym`, `.gnu.version`, `.gnu.version_r` and
  `.gnu.version_d`.

### `hobjdump`

```
./hobjdump [objfile ...]
```

Prints, for each file, the same header and section hex dump as
`objdump -sf`:

* `file format` line, derived from the ELF class/endianness/machine.
* `architecture: ..., flags 0x........:` line followed by the matching
  flag names (`HAS_RELOC`, `EXEC_P`, `HAS_SYMS`, `DYNAMIC`, `D_PAGED`),
  reconstructed from `e_type`, `e_phnum`, and the presence of a symbol
  table — `objdump` does not read these from `e_flags`, it derives a
  synthetic BFD flag word.
* `start address`.
* `Contents of section <name>:` hex+ASCII dump for every section that
  actually carries file content (skips `.bss`-like `SHT_NOBITS`, the
  section header string table, `.symtab` and its linked `.strtab`, and,
  for relocatable files only, `.rel`/`.rela` sections — matching what BFD
  exposes as ordinary "sections" versus what it consumes internally).

## Design

Both tools share `elf_common.{h,c}` / `elf_load.c`: a small set of
endian- and width-aware accessors that read ELF32/ELF64 header, section
header and symbol entries into generic 64-bit-wide in-memory structures
(`sect_t`, `sym_t`), so the rest of the code never has to branch on class
or byte order again.

## Build

```
make hnm
make hobjdump
```

or `make` / `make all` for both. `make clean` removes the binaries.

## Limitations

* The `objdump -sf` architecture/file-format name table only covers the
  common machines (x86, x86-64, ARM, AArch64, MIPS, PowerPC, SPARC,
  RISC-V); unrecognized `e_machine` values fall back to the generic
  `elfNN-little`/`elfNN-big` BFD target names.
* Tested for exact byte-for-byte match against the host's GNU `nm`/
  `objdump` on real 64-bit little-endian binaries (executables, PIE,
  shared objects, relocatable `.o`, stripped binaries) and against
  hand-crafted 32/64-bit, little/big-endian ELF files.
