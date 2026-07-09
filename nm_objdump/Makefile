CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

NM_SRC = elf_common.c elf_load.c hnm_main.c hnm_symtab.c hnm_version.c hnm_print.c
OBJDUMP_SRC = elf_common.c elf_load.c hobjdump_main.c hobjdump_header.c hobjdump_sections.c

all: hnm hobjdump

hnm: $(NM_SRC)
	$(CC) $(CFLAGS) $(NM_SRC) -o hnm

hobjdump: $(OBJDUMP_SRC)
	$(CC) $(CFLAGS) $(OBJDUMP_SRC) -o hobjdump

clean:
	rm -f hnm hobjdump

.PHONY: all clean
