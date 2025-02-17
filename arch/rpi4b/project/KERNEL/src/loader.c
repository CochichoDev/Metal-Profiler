#include <ctype.h>

#include "common.h"
#include "loader.h"
#include "uart.h"

#define DEBUG

extern u64 rst_addr;

static u8 state = RUN;

static char *get_next_space(char *str) {
    while (*str != ' ' && *str != '\n') ++str;
    return str;
}

static char *get_next_char(char *str) {
    while ((!isalpha(*str)) && *str != '\n') ++str;
    return str;
}

static char *get_next_digit(char *str) {
    while ((!isdigit(*str)) && *str != '\n') ++str;
    return str;
}

#define __arch64__

void loader_load_section() {
    /* Protocol 
     * u32/u64  Size of Section
     * u32/u64  VMA of Section
     * u8[]     Data of Section
     */
    u8 num_bytes;
    u8 *ptr;
#if defined(__arch32__)
    u32 size;
    num_bytes = 4;
#elif defined(__arch64__)
    u64 size;
    num_bytes = 8;
#endif

    char buffer[num_bytes];
    for (u8 byte_idx = 0; byte_idx < num_bytes; ++byte_idx) {
        buffer[byte_idx] = inbyte();
    }
#if defined(__arch32__)
    size = *( (u32 *)buffer );
#ifdef DEBUG
    uart_str("Section size: ");
    uart_hex(size);
    uart_nl();
#endif
#elif defined(__arch64__)
    size = *( (u64 *)buffer );
#ifdef DEBUG
    uart_str("Section size: ");
    uart_hex(size);
    uart_nl();
#endif
#endif

    for (u8 byte_idx = 0; byte_idx < num_bytes; ++byte_idx) {
        buffer[byte_idx] = inbyte();
    }
#if defined(__arch32__)
    ptr = (u8 *) *( (u32 *)buffer );
#ifdef DEBUG
    uart_str("Section addr: ");
    uart_hex((u32)ptr);
    uart_nl();
#endif
#elif defined(__arch64__)
    ptr = (u8 *) *( (u64 *)buffer );
#ifdef DEBUG
    uart_str("Section addr: ");
    uart_hex((u64)ptr);
    uart_nl();
#endif
#endif

    while (size-- > 0) {
        *ptr++ = inbyte();
    }
}

u8 loader_handle_entry(void *base_addr) {
    // Send Protocol Header
    /*
     *  u8 -> Core Number that the elf is supposed to run on
     *  u32/u64 -> Entry address for the elf execution
     *  u8 -> Number of sections to load
     */
    
    u8 core_num = inbyte();
    u8 bytes_per_addr;

#if defined(__arch32__)
    bytes_per_addr = 4;
#elif defined(__arch64__)
    bytes_per_addr = 8;
#else
    #error "The platform architecture needs to be specified"
#endif

    u8 *addr = ( (u8 *)base_addr ) + (core_num * bytes_per_addr);;
    for (u8 input = 0; input < bytes_per_addr; ++input) {
        addr[input] = inbyte();
    }

    // Return the next byte == Number of sections
    return inbyte();
}

void loader_scan_action() {
    uart_str("LOADER STARTED\n\r");
    char input[64];
    char *ptr1;
    while (state) {
        uart_str("> ");
        ptr1 = input;
        get_uart_input(input, 64);
        ptr1 = get_next_char(ptr1);
    
        if (!strncmp(ptr1, "LOAD", 4)) {
            u8 num_sections = loader_handle_entry(&rst_addr);
            for (u8 sec_idx = 0; sec_idx < num_sections; ++sec_idx) {
                loader_load_section();
            }

        } else if (!strncmp(ptr1, "RUN", 3)) {
            __asm__ __volatile__("sev");

        } else if (!strncmp(ptr1, "INFO", 4)) {
            uart_str("INFO"); uart_nl();
            uart_str("EP0: "); uart_hex(rst_addr); uart_nl();
            uart_str("EP1: "); uart_hex(*(&rst_addr + 1)); uart_nl();
            uart_str("EP2: "); uart_hex(*(&rst_addr + 2)); uart_nl();
            uart_str("EP3: "); uart_hex(*(&rst_addr + 3)); uart_nl();

        } else if (!strncmp(ptr1, "EXIT", 4)) {
            uart_str("EXITING...\n\r");
            state = STOP;
        } else {
            uart_str("UNRECOGNIZED ACTION\n\r");

        }
    }
}
