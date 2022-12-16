#ifndef __ELF_H
#define __ELF_H
#include "stdint.h"

struct __attribute__((__packed__)) elf_header {
    uint8_t magic[4];
    uint8_t class;
    uint8_t endian;
    uint8_t version;
    uint8_t osabi;
    uint8_t abiversion;
    char pad[7];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
};

struct __attribute__((__packed__)) elf_program_header {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
};


struct __attribute__((__packed__)) elf_section_header {
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t shflags;
    uint32_t sh_addr;
    uint32_t sh_offset;
    uint32_t s_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint32_t sh_addralign;
    uint32_t sh_entsize;
};


struct __attribute__((__packed__)) elf_full {
    struct elf_header header;
    struct elf_program_header *prog_headers;
    struct elf_section_header *sec_headers;
};

enum elf_error {
    ELF_OK,
    ELF_NOT_ELF,
    ELF_UNSPECIFIED,
};

enum elf_error elf_parse_header(uint8_t *buffer, struct elf_header *header);
enum elf_error elf_parse_program_header(uint8_t *buffer, struct elf_program_header *header);
enum elf_error elf_parse_section_header(uint8_t *buffer, struct elf_section_header *header);
enum elf_error elf_parse_full(uint8_t *buffer, struct elf_full *elf);
#endif
