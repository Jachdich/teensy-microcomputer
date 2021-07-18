#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct __attribute__((__packed__)) {
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
} ELF_header;

typedef struct __attribute__((__packed__)) {
    
} ELF_program_header;

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("%s: Requires one argument: filename\n", argv[0]);
        return 1;
    }
    FILE* fp = fopen(argv[1], "rb");

    if (fp == NULL) {
        printf("%s: Error opening elf file '%s'\n", argv[0], argv[1]);
        return 1;
    }
    
    fseek(fp, 0, SEEK_END);
    size_t fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    uint8_t *contents = malloc(fsize + 1);
    fread(contents, 1, fsize, fp);
    contents[fsize] = 0;
    fclose(fp);
    
    if (contents[0] != 0x7f || contents[1] != 'E' || contents[2] != 'L' || contents[3] != 'F') {
        printf("%s: File '%s' is not an ELF file!\n", argv[0], argv[1]);
        return 1;
    }

    ELF_header header;
    memcpy(&header, contents, 0x34);
    printf("%d\n", header.e_entry);
    
    return 0;
}