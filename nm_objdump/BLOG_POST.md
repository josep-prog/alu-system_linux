# Inside the ELF: How Linux Turns Bytes Into Running Programs

![ELF file structure diagram — ELF Header, Program Header Table, Sections, Section Header Table](PASTE_YOUR_IMAGE_HERE.png)
*Figure: high-level layout of an ELF file — header, program headers, sections, and the section header table.*

> Add an image at the very top before publishing. A simple diagram of the ELF layout (header → program headers → sections → section header table) works well. You can sketch one yourself, screenshot a diagram from the ELF Wikipedia page, or generate one — just make sure it's at the top of the post on both Medium and LinkedIn, since the mobile editor on LinkedIn can sometimes strip the first image if it's added a certain way.

Every time you run a program on Linux — `ls`, `bash`, your own compiled `a.out` — the kernel has to answer a basic question: *what is in this file, and how do I load it into memory and start executing it?* The answer is encoded in a single, very deliberate binary format: **ELF**, the Executable and Linkable Format.

In this post I'll walk through what ELF is, why it exists, what it stores, how that data is laid out on disk, how you can parse it yourself, and how three classic Unix tools — `readelf`, `nm`, and `objdump` — read it. Along the way I'll use a pair of tools I wrote from scratch (`hnm` and `hobjdump`, reimplementations of `nm` and `objdump`) to show the parsing in practice.

## What is ELF?

ELF (Executable and Linkable Format) is the standard binary file format used on Linux and most other Unix-like systems for:

- **Executables** — programs you can run directly (`ET_EXEC`, or `ET_DYN` for position-independent executables)
- **Relocatable object files** — the `.o` files produced by a compiler before linking (`ET_REL`)
- **Shared libraries** — `.so` files (`ET_DYN`)
- **Core dumps** — memory snapshots of a crashed process (`ET_CORE`)

It replaced older formats like a.out and COFF in the early 1990s specifically because those formats couldn't cleanly support dynamic linking, multiple architectures, and debugging information at the same time. ELF was designed by the Unix System Laboratories as part of the System V Release 4 ABI and was built to be **extensible and architecture-independent** — the same format describes a 32-bit ARM binary and a 64-bit x86-64 binary; only the field widths and a few flags differ.

## Why ELF is used

A binary format needs to satisfy several audiences at once:

1. **The kernel loader** needs to know how to map the file into memory, where the entry point is, and which segments need which permissions (read/write/execute).
2. **The dynamic linker (`ld.so`)** needs to know which shared libraries this file depends on, and which symbols it imports and exports.
3. **The linker (`ld`)** combining multiple `.o` files needs section-level information — code, data, relocations — to merge files together.
4. **Debuggers and analysis tools** need symbol tables and (optionally) debug info to map addresses back to function/variable names.

ELF satisfies all four with one format by offering **two parallel views of the same file**: a *segment* view (via the Program Header Table) for what the loader needs at runtime, and a *section* view (via the Section Header Table) for what the linker and tools need at build/analysis time. This dual-view design — plus first-class support for dynamic symbol tables and relocations — is why ELF won out and is still the standard nearly 30 years later.

## What information is stored in an ELF file

An ELF file is a flat binary blob with these key pieces:

### 1. Object file architecture (the ELF header)
The first 64 bytes (52 on 32-bit) describe the file itself:
- Magic number (`0x7f 'E' 'L' 'F'`)
- Class: 32-bit or 64-bit (`ELFCLASS32` / `ELFCLASS64`)
- Data encoding: little-endian or big-endian
- File type: relocatable, executable, shared object, or core
- Target machine (x86-64, ARM, MIPS, etc.)
- Entry point address — where execution begins
- Offsets and counts for the program header table and section header table

### 2. Segments and sections
- **Program headers** describe *segments* — contiguous chunks the loader maps into memory (`PT_LOAD`), the dynamic linking info (`PT_DYNAMIC`), the interpreter path (`PT_INTERP`), etc.
- **Section headers** describe *sections* — `.text` (code), `.data` (initialized globals), `.bss` (uninitialized globals, takes no file space), `.rodata` (constants), `.symtab`/`.dynsym` (symbol tables), `.rela.*` (relocations), `.comment`, and many more. Each section header records a name, type, flags, virtual address, file offset, size, alignment, and (for tables) entry size.

### 3. Symbol tables
- `.symtab` holds the **full symbol table** — every function and global variable the compiler/linker knows about, with a name, value (address), size, type (function, object, etc.), binding (local, global, weak), and the section it belongs to.
- `.dynsym` holds the **dynamic symbol table** — the subset of symbols needed at runtime for dynamic linking (imported symbols from shared libraries, and symbols this file exports). A statically-linked stripped binary may have no `.symtab` at all, but a dynamically-linked one will almost always have a `.dynsym`.

Here's `.dynsym` in a real binary, dumped by my own `hobjdump`:

```
Contents of section .dynsym:
 4002b8 00000000 00000000 00000000 00000000  ................
 4002c8 00000000 00000000 4b000000 12000000  ........K.......
 4002d8 00000000 00000000 00000000 00000000  ................
 ...
```

## How this information is stored

ELF doesn't just dump structs into a file arbitrarily — everything is organized into **header tables**, and every field has a fixed, well-defined width that depends on whether the file is 32-bit or 64-bit:

| Field example          | 32-bit width | 64-bit width |
|-------------------------|:---:|:---:|
| Addresses (`Elf_Addr`)  | 4 bytes | 8 bytes |
| Offsets (`Elf_Off`)     | 4 bytes | 8 bytes |
| Section/segment sizes   | 4 bytes | 8 bytes |
| `half`/`word` fields (type, machine, flags) | same on both | same on both |

So the *same logical structure* (`Elf32_Ehdr` vs `Elf64_Ehdr`, `Elf32_Shdr` vs `Elf64_Shdr`, `Elf32_Sym` vs `Elf64_Sym`) exists in two sizes, and the `EI_CLASS` byte at offset 4 of the file tells a parser which one to use. Endianness is similarly explicit: the `EI_DATA` byte tells you whether multi-byte integers are little- or big-endian, so a parser has to byte-swap fields when the file's endianness doesn't match the host's.

The layout, conceptually:

```
[ELF Header]
[Program Header 1][Program Header 2]...   <- segments (runtime view)
[.text][.rodata][.data][.bss]...           <- raw section bytes
[.symtab][.dynsym][.strtab]...             <- symbol/string tables
[Section Header 1][Section Header 2]...    <- sections (link/analysis view)
```

The header tables themselves are just **arrays of fixed-size C structs**, located at the file offsets given in the ELF header (`e_phoff`, `e_shoff`), with `e_phnum`/`e_shnum` entries of `e_phentsize`/`e_shentsize` bytes each.

## How to parse an ELF file

The good news: you don't need to guess the struct layouts — they're already defined for you in **`/usr/include/elf.h`**. That single header gives you everything:

```c
#include <elf.h>

Elf64_Ehdr ehdr;   /* the file header           */
Elf64_Shdr shdr;   /* one section header entry  */
Elf64_Sym  sym;    /* one symbol table entry    */
```

A minimal parser:

1. `open()`/`mmap()` (or `read()`) the file into memory.
2. Check the magic bytes (`ELFMAG`, defined in `elf.h`) and read `e_ident[EI_CLASS]` to decide whether to interpret everything as `Elf32_*` or `Elf64_*`.
3. Cast the start of the buffer to `Elf32_Ehdr *` / `Elf64_Ehdr *` and read `e_shoff`/`e_shnum`/`e_shentsize` to locate the section header table.
4. Walk the section header array; for each `Elf64_Shdr`, `sh_name` is an *index into the `.shstrtab` string section*, not a string itself — you resolve it by adding it to the base of the string table section's bytes.
5. To read symbols, find the `.symtab`/`.dynsym` section, then walk it as an array of `Elf64_Sym`, resolving each symbol's `st_name` against its associated string table (`.strtab`/`.dynstr`) the same way.
6. If the host's endianness doesn't match `e_ident[EI_DATA]`, byte-swap every multi-byte field as you read it.

That's the entire algorithm behind `readelf`, `nm`, and `objdump` — and behind my own reimplementations, `hnm` and `hobjdump`. For example, here's `hnm` walking `.symtab`/`.dynsym` on its own `hobjdump` binary:

```
$ ./hnm hobjdump
00000000000036b8 r __abi_tag
0000000000001130 t deregister_tm_clones
0000000000001160 t register_tm_clones
00000000000011a0 t __do_global_dtors_aux
0000000000005088 b completed.0
...
00000000000013fe t parse_ehdr
0000000000001754 t fill_shdr
00000000000019ec t parse_shdrs
```

Notice the symbol type letters (`t` = text/function, `b` = BSS, `r` = read-only data, lowercase = local binding, uppercase = global) — that convention comes straight from `nm`, and it's derived purely from each symbol's `st_info` (type + binding) and `st_shndx` (which section it lives in).

## The `readelf` command

`readelf` is the most complete, low-level ELF inspector. It prints the raw structures almost verbatim: the ELF header, program headers, section headers, dynamic section, relocations, and symbol tables. It's architecture-aware (works on any ELF, regardless of the host's own architecture) and is the tool you reach for when you want to see *exactly* what's in the file, field by field.

```
$ readelf -h hobjdump
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF64
  Data:                              2's complement, little endian
  Type:                              DYN (Position-Independent Executable file)
  Machine:                           Advanced Micro Devices X86-64
  Entry point address:               0x1100
  Start of program headers:          64 (bytes into file)
  Start of section headers:          19600 (bytes into file)
  Number of program headers:         14
  Number of section headers:         32
  Section header string table index: 31
```

Common flags: `-h` (file header), `-l` (program/segment headers), `-S` (section headers), `-s` (symbol table), `-d` (dynamic section), `-r` (relocations).

## The `nm` command

`nm` lists the **symbol table** of an object file, executable, or shared library — one line per symbol, showing its address, type, and name. It's the quickest way to answer "does this binary define/use symbol X?" without dragging in the rest of the ELF structure. It reads `.symtab` by default and `.dynsym` for files that only have a dynamic symbol table.

```
$ nm hobjdump | grep parse
0000000000001754 t fill_shdr
00000000000019ec t parse_shdrs
00000000000013fe t parse_ehdr
```

## The `objdump` command

`objdump` is the Swiss-army knife of the three — it can disassemble machine code, dump section contents as hex, print the file/header summary, show relocations, and more. The combination `objdump -sf` (which my `hobjdump` reproduces) prints the `-f` summary (architecture, flags, entry point) followed by `-s`, a full hex+ASCII dump of every section's raw bytes:

```
$ objdump -sf hobjdump

hobjdump:     file format elf64-x86-64
architecture: i386:x86-64, flags 0x00000150:
HAS_SYMS, DYNAMIC, D_PAGED
start address 0x0000000000001100

Contents of section .interp:
 318 2f6c6962 36342f6c 642d6c69 6e75782d  /lib64/ld-linux-
 328 7838362d 36342e73 6f2e3200           x86-64.so.2.
...
```

## `nm` vs `objdump` vs `readelf`

- **`readelf`** is the most complete and structural — it shows you every ELF data structure as-is.
- **`nm`** is the most focused — symbols only, nothing else, which makes it ideal for scripting and quick lookups.
- **`objdump`** is the most versatile — it can show you everything `readelf` can plus raw section bytes *and* disassembled instructions, making it the tool of choice when you need to go from "what sections exist" to "what machine code is actually in them."

## Closing thought

ELF earns its longevity by separating concerns cleanly: one header to identify the file, one table for what the *loader* cares about, one table for what the *linker/analysis tools* care about, and dedicated tables for symbols and strings — all using fixed-width structs you can find verbatim in `/usr/include/elf.h`. Once you understand that layout, tools like `readelf`, `nm`, and `objdump` stop looking like magic and start looking like exactly what they are: straightforward struct walkers over a very well-specified file format.

---

*Reimplementations of `nm` and `objdump` referenced in this post (`hnm`, `hobjdump`) are available on GitHub: [alu-system_linux/nm_objdump](#).*
