#include "elf.h"
#include "jstdlib.h"

enum elf_error elf_parse_header(uint8_t *buffer, struct elf_header *header) {
    if (buffer[0] != 0x7f || buffer[1] != 'E' || buffer[2] != 'L' || buffer[3] != 'F') {
        return ELF_NOT_ELF;
    }

    memcpy(header, buffer, 0x34);
    return ELF_OK;
}

enum elf_error elf_parse_program_header(uint8_t *buffer, struct elf_program_header *ph) {
    memcpy(ph, buffer, 0x20);
    return ELF_OK;
}

enum elf_error elf_parse_section_header(uint8_t *buffer, struct elf_section_header *sh) {
    memcpy(sh, buffer, 0x28);
    return ELF_OK;
}

enum elf_error elf_parse_full(uint8_t *buffer, struct elf_full *elf) {
    enum elf_error ret = elf_parse_header(buffer, &elf->header);
    if (ret != ELF_OK) return ret;
    elf->prog_headers = malloc(sizeof(*elf->prog_headers) * elf->header.e_phnum);
    for (uint32_t i = 0; i < elf->header.e_phnum; i++) {
        struct elf_program_header ph;
        ret = elf_parse_program_header(buffer + elf->header.e_phoff + i * 0x20, &ph);
        elf->prog_headers[i] = ph;
        if (ret != ELF_OK) {
            elf_free(elf);
            return ret;
        }
        
    }
    elf->sec_headers = malloc(sizeof(*elf->sec_headers) * elf->header.e_shnum);
    for (uint32_t i = 0; i < elf->header.e_shnum; i++) {
        struct elf_section_header sh;
        ret = elf_parse_section_header(buffer + elf->header.e_shoff + i * 0x20, &sh);
        elf->sec_headers[i] = sh;
        if (ret != ELF_OK) {
            elf_free(elf);
            return ret;
        }
    }

    
}

void elf_free(struct elf_full *elf) {
    if (elf->prog_headers != NULL) free(elf->prog_headers);
    if (elf->sec_headers  != NULL) free(elf->sec_headers);
}
