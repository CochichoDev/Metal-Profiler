#include "common.h"
#include "loader.h"
#include "uart.h"
#include "utils.h"
#include "gic.h"
#include "gpio.h"
#include "asm.h"
#include "power.h"
#include "mbox.h"

//#define DEBUG

extern u64 rst_addr;
extern u64 proc_hang;
extern u64 table_addr;

static u8 state = RUN;

void dispatch_routine();
void configure_mmu();
void disable_mmu();
void clean_invalidate_dcache();

struct mem_block {
    u8 *addr;
    #if defined(__aarch32__)
    u32 size;
    #elif defined(__aarch64__)
    u64 size;
    #endif
};

static struct mem_block alloc_mmu[64];
static struct mem_block alloc_mem[128];

static u8 num_mem_blocks = 0;
static u8 num_mmu_blocks = 0;

static char *get_next_space(char *str) {
    while (*str != ' ' && *str != '\n') ++str;
    return str;
}

static char *get_next_char(char *str) {
    while ((!ischar(*str)) && *str != '\n') ++str;
    return str;
}

static char *get_next_digit(char *str) {
    while ((!isdigit(*str)) && *str != '\n') ++str;
    return str;
}

static void reset_mem(struct mem_block *blocks, u8 num_blocks) {
    for (u8 block_idx = num_blocks; block_idx > 0; block_idx--) {
        #ifdef DEBUG
        uart_str("Cleaning block number "); uart_int(block_idx-1); 
        uart_str(" at address "); uart_hex((u64) blocks[block_idx-1].addr);
        #endif
        #if defined(__aarch32__)
        u32 size = blocks[block_idx-1].size;
        #elif defined(__aarch64__)
        u64 size = blocks[block_idx-1].size;
        #endif
        #ifdef DEBUG
        uart_str(" of size "); uart_int(size); uart_nl();
        #endif
        for (u8 *ptr = blocks[block_idx-1].addr; size > 0; size--) ptr[size-1] = 0;
    }
}

#define __arch64__

void loader_load_section(struct mem_block *blocks, u8 *num_blocks) {
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
    blocks[*num_blocks].addr = ptr;
    blocks[*num_blocks].size = size;
    *num_blocks += 1;

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

    u8 buffer_addr[bytes_per_addr];
    for (u8 input = 0; input < bytes_per_addr; ++input) {
        buffer_addr[input] = inbyte();
    }
    if (core_num > 200) {
        u8 num_cores = 255 - core_num;
        for (u8 core_idx = 0; core_idx < num_cores; core_idx++) {
            u8 *addr = ( (u8 *)base_addr ) + (core_idx * bytes_per_addr);;
            for (u8 input = 0; input < bytes_per_addr; ++input) {
                addr[input] = buffer_addr[input];
            }
        }
    } else {
        u8 *addr = ( (u8 *)base_addr ) + (core_num * bytes_per_addr);;
        for (u8 input = 0; input < bytes_per_addr; ++input) {
            addr[input] = buffer_addr[input];
        }
    }

    // Return the next byte == Number of sections
    return inbyte();
}

void print_mem_blocks(struct mem_block *blocks, u8 num_blocks) {
    for (size_t block_idx = 0; block_idx < num_blocks; block_idx++) {
        uart_str("Block Address: "); uart_hex((u64) blocks[block_idx].addr); uart_nl();
        uart_str("Block Size: "); uart_hex(blocks[block_idx].size); uart_nl();
        
        /*
        for (size_t byte_idx = 0; byte_idx < alloc_mem[block_idx].size; byte_idx++) {
            uart_hex((u8) alloc_mem[block_idx].addr[byte_idx]); uart_str(" ");
        }
        uart_nl();
        */
    }
}

void loader_scan_action() {
    uart_str("LOADER STARTED"); uart_nl();
    char input[64];
    char *ptr1;
    while (state) {
        uart_str("> ");
        outbyte(0x0);
        ptr1 = input;
        get_uart_input(input, 64);
        ptr1 = get_next_char(ptr1);

        if (!strncmp(ptr1, "LOAD", 4)) {
            /* Charge ELF */
            u8 num_sections = loader_handle_entry(&rst_addr);
            for (u8 sec_idx = 0; sec_idx < num_sections; ++sec_idx) {
                loader_load_section(alloc_mem, &num_mem_blocks);
            }

        } else if (!strncmp(ptr1, "MMU", 3)) {
            /* Charge MMU ELF */
            u8 num_sections_mmu = loader_handle_entry(&table_addr);
            for (u8 sec_idx = 0; sec_idx < num_sections_mmu; ++sec_idx) {
                loader_load_section(alloc_mmu, &num_mmu_blocks);
            }
            configure_mmu();        // Checks if the table_addr has changed and applies the table if so

        } else if (!strncmp(ptr1, "RUN", 3)) {
            __asm__ __volatile__("sev");

            dispatch_routine();
            wait(5000000);

            /* Send End Execution SGI's */
            //*REG_GICD_SGIR = (0 << 24) | (0xE << 16) | 0x0;


        } else if (!strncmp(ptr1, "INFO", 4)) {
            uart_str("INFO"); uart_nl();
            uart_str("EP0: "); uart_hex(rst_addr); uart_str(" MMU Table: "); uart_hex(table_addr); uart_nl();
            uart_str("EP1: "); uart_hex(*(&rst_addr + 1)); uart_str(" MMU Table: "); uart_hex(*(&table_addr + 1)); uart_nl();
            uart_str("EP2: "); uart_hex(*(&rst_addr + 2)); uart_str(" MMU Table: "); uart_hex(*(&table_addr + 2)); uart_nl();
            uart_str("EP3: "); uart_hex(*(&rst_addr + 3)); uart_str(" MMU Table: "); uart_hex(*(&table_addr + 3)); uart_nl();

        } else if (!strncmp(ptr1, "SYS", 3)) {
            uart_str("SYSTEM INFO:"); uart_nl();
            uart_str("ARM FREQ: "); uart_int(mbox_get_arm_freq()); uart_nl();
            uart_str("UART FREQ: "); uart_int(mbox_get_uart_freq()); uart_nl();
            uart_str("CORE FREQ: "); uart_int(mbox_get_core_freq()); uart_nl();
            u32 cntfrq_value = 0;
            asm volatile("  mrs x0, CNTFRQ_EL0  \n\
                            mov %0, x0" : "=r" (cntfrq_value) : : "x0", "memory");
            uart_str("CNTFRQ_EL0: "); uart_int(cntfrq_value); uart_nl();

        } else if (!strncmp(ptr1, "RESET", 5)) {
            /* Reset MMU Table addresses */
            table_addr = 0;
            *(&table_addr + 1) = 0;
            *(&table_addr + 2) = 0;
            *(&table_addr + 3) = 0;

            /* Reset rst_address */
            rst_addr = (u64) &proc_hang;
            *(&rst_addr + 1) = (u64) &proc_hang;
            *(&rst_addr + 2) = (u64) &proc_hang;
            *(&rst_addr + 3) = (u64) &proc_hang;

            reset_mem(alloc_mem, num_mem_blocks);
            /* Disable MMU before erasing pages */
            asm volatile("msr SCTLR_EL3, xzr");
            asm volatile("dmb sy");
            asm volatile("isb");
            reset_mem(alloc_mmu, num_mmu_blocks);
            num_mem_blocks = 0;
            num_mmu_blocks = 0;

            outbyte(0x0);
            asm volatile("dmb sy");
            asm volatile("isb");
            pm_reset();

        } else if (!strncmp(ptr1, "MEM", 3)) {
            print_mem_blocks(alloc_mmu, num_mmu_blocks);
            print_mem_blocks(alloc_mem, num_mem_blocks);

        } else if (!strncmp(ptr1, "EXIT", 4)) {
            uart_str("EXITING...\n\r");
            state = STOP;
        } else {
            uart_str("UNRECOGNIZED ACTION\n\r");

        }
    }
    input[0] = 0;
}
