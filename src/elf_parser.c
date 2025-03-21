#include <endian.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "uart.h"
#include "elf_reader.h"
#include "common.h"

#define DEBUG

err process_elf_32(s32 fd, u8 core_num) {
    // Reinitialize the file cursor
    lseek(fd, 0, SEEK_SET);

    // Read ELF header
    Elf32_Ehdr header;
    read(fd, &header, sizeof(header));

    // Read Program headers
    Elf32_Shdr *sheaders = malloc(sizeof(Elf32_Shdr) * header.e_shnum);
    lseek(fd, header.e_shoff, SEEK_SET);
    read(fd, sheaders, sizeof(Elf32_Shdr) * header.e_shnum);

    for (size_t sidx = 0; sidx < header.e_shnum; ++sidx) {
        printf("File Offset: 0x%x\n", sheaders[sidx].sh_offset);
        printf("Size: 0x%x\n", sheaders[sidx].sh_size);
        printf("Virtual Address: 0x%x\n", sheaders[sidx].sh_addr);
        printf("Section is %sloadable\n", (sheaders[sidx].sh_flags & SHF_ALLOC) ? "" : "not ");
    }

    return 0;
}

err process_elf_64(s32 fd, u8 core_num) {
    // Reinitialize the file cursor
    lseek(fd, 0, SEEK_SET);

    // Read ELF header
    Elf64_Ehdr header;
    read(fd, &header, sizeof(header));

    // Read Program headers
    Elf64_Shdr *sheaders = malloc(sizeof(Elf64_Shdr) * header.e_shnum);
    lseek(fd, header.e_shoff, SEEK_SET);
    read(fd, sheaders, sizeof(Elf64_Shdr) * header.e_shnum);

    // Send Protocol Header
    /*
     *  u8 -> Core Number that the elf is supposed to run on
     *  u32/u64 -> Entry address for the elf execution
     *  u8 -> Number of sections to load
     */
    uart_send_byte(core_num);
    for (u8 addr_idx = 0; addr_idx < 8; addr_idx++) {
        uart_send_byte( *(((u8 *)&header.e_entry) + addr_idx) );
    }
    u8 total_loadable_sections = 0;
    for (size_t sidx = 0; sidx < header.e_shnum; ++sidx) {
        printf("File Offset: 0x%lx\n", sheaders[sidx].sh_offset);
        printf("Size: 0x%lx\n", sheaders[sidx].sh_size);
        printf("Virtual Address: 0x%lx\n", sheaders[sidx].sh_addr);
        if ((sheaders[sidx].sh_flags & SHF_ALLOC) == 0 || (sheaders[sidx].sh_type == SHT_NOBITS)) continue;
        printf("Section is loadable\n\n");
        ++total_loadable_sections;
    }

    uart_send_byte(total_loadable_sections);

    
    for (size_t sidx = 0; sidx < header.e_shnum; ++sidx) {
        if ((sheaders[sidx].sh_flags & SHF_ALLOC) == 0 || (sheaders[sidx].sh_type == SHT_NOBITS)) continue;

        printf("Sending data...\n");
        /*  Send SECTION metadata  */
        for (u8 size_idx = 0; size_idx < sizeof(sheaders[sidx].sh_size); ++size_idx) {
            uart_send_byte(*( ((u8 *) &sheaders[sidx].sh_size) + size_idx));
        }
        for (u8 addr_idx = 0; addr_idx < sizeof(sheaders[sidx].sh_addr); ++addr_idx) {
            uart_send_byte(*( ((u8 *) &sheaders[sidx].sh_addr) + addr_idx));
        }
        
        /*  Send SECTION data  */
        lseek(fd, sheaders[sidx].sh_offset, SEEK_SET);
        u8 data_buf;
        for (u64 data_idx = 0; data_idx < sheaders[sidx].sh_size; ++data_idx) {
            read(fd, &data_buf, sizeof(data_buf));
            uart_send_byte(data_buf);
        }
    }

    return 0;
}

err open_elf(const char *path, u8 core_num) {
    s32 fd = open(path, 0, O_RDONLY);

    u8 magic_numbers[] = {0x7f,'E','L','F',0};
    u8 header[sizeof(magic_numbers)];

    read(fd, header, sizeof(header));

    size_t idx = 0;
    for (; idx < 4; ++idx) {
        if (magic_numbers[idx] != header[idx]) break;
    }

    if (idx != 4) {
        fprintf(stderr, "Error: The file is not in ELF format\n");
        return -1;
    }

#ifdef DEBUG
    printf("File correctly identified as ELF\n");
#endif

    err rvalue = 0;

    if (header[4] == 1) {
#ifdef DEBUG
        printf("Corresponding ELF is 32 bit class\n");
#endif
        rvalue = process_elf_32(fd, core_num);
    } else {
#ifdef DEBUG
        printf("Corresponding ELF is 64 bit class\n");
#endif
        rvalue = process_elf_64(fd, core_num);
    }

    return rvalue;
}


