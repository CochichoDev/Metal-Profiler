#include "uart.h"
#include "mbox.h"

//#define DEBUG

static void mbox_create_request(u32 buffer[], u32 tag, u32 req_size, u32 req[], u32 res_size) {
    /* MAKE SURE BOTH SIZES ARE 4 BYTE ALIGNED */
    req_size = (req_size & 0x2) ? ( (req_size + 0x2) & ~(0x2U) ) : req_size ;
    res_size = (res_size & 0x2) ? ( (res_size + 0x2) & ~(0x2U) ) : res_size ;

    u32 ptr = 0;
    buffer[ptr++] = 0;              // Buffer Total Size
    buffer[ptr++] = 0;              // Buffer Request Code (Always 0)

    buffer[ptr++] = tag;            // Tag
    buffer[ptr++] = (req_size > res_size) \
                    ? req_size : res_size;  // Size of Buffer
    buffer[ptr++] = 0;              // Tag Code

    for ( ; ptr < (req_size >> 2) + 5; ++ptr) buffer[ptr] = req[ptr-5];
    ptr = 5 + buffer[3];

    buffer[ptr++] = 0;              // Zero Tag
    buffer[0] = ptr << 2;
}

static u32 mbox_get_response(u32 response[], u32 buffer[]) {
    u32 ptr = 0;
    for ( ; ptr < ( buffer[4] & ~(0x80000000) ); ++ptr) {
        response[ptr] = buffer[ptr+5];
    }
    
    return ptr;
}

static u32 mbox_get_info(u32 response[], u32 tag, u32 req_size, u32 req[], u32 res_size) {
#ifdef DEBUG
    uart_str("Get info function\n");
#endif

    __attribute__((aligned(16))) u32 buffer[16];

    mbox_create_request(buffer, tag, req_size, req, res_size);

    while ( (*REG_MBOX_WR_ST & MBOX_ST_FULL_MASK) );

    uintptr_t mail = (uintptr_t) buffer;

#ifdef DEBUG
    uart_str("Sending the following mail: "); uart_hex(mail); uart_nl();
    uart_str("Status Register: "); uart_hex(*REG_MBOX_WR_ST); uart_nl();
#endif

    mail &= ~(0xFU);
    mail |= 0x8;

    *REG_MBOX_WR = mail;

#ifdef DEBUG
    uart_str("Waiting for response: "); uart_hex(mail); uart_nl();
    uart_str("Status Register: "); uart_hex(*REG_MBOX_WR_ST); uart_nl();
#endif
    
    while ( (*REG_MBOX_RD_ST & MBOX_ST_EMPTY_MASK) );

#ifdef DEBUG
    uart_str("Message received\n");
    uart_str("Status Register: "); uart_hex(*REG_MBOX_RD_ST); uart_nl();
#endif

    mail = *REG_MBOX_RD;

    if ((mail & 0xF) != 0x8) {
    #ifdef DEBUG
        uart_str("Could not retrieve info from VPU mailbox\n");
    #endif
        return 0;
    }

    return mbox_get_response(response, buffer);
}

u32 mbox_get_core_freq() {
    u32 response[16];

    if (mbox_get_info(response, 0x00030002, 4, (u32 []) {0x4}, 8) < 8 ||
        response[0] != 0x4) return 0;

#ifdef DEBUG
    uart_str("ARM Freq: "); uart_int(response[1]); uart_nl();
#endif

    return response[1];
}

u32 mbox_get_arm_freq() {
    u32 response[16];

    if (mbox_get_info(response, 0x00030002, 4, (u32 []) {0x3}, 8) < 8 ||
        response[0] != 0x3) return 0;

#ifdef DEBUG
    uart_str("ARM Freq: "); uart_int(response[1]); uart_nl();
#endif

    return response[1];
}

u32 mbox_set_arm_freq(u32 freq, u32 disable_turbo) {
    u32 response[16];

    if (mbox_get_info(response, 0x00038002, 12, (u32 []) {0x3U,freq,disable_turbo}, 8) < 8 ||
        response[0] != 0x3) return 0;

#ifdef DEBUG
    uart_str("ARM Freq: "); uart_int(response[1]); uart_nl();
#endif

    return response[1];
}

u32 mbox_get_uart_freq() {
    u32 uart_response[16];
    
    if (mbox_get_info(uart_response, 0x00030002, 4, (u32 []) {0x2}, 8) < 8 ||
        uart_response[0] != 0x2) {
        return 0;
    }

#ifdef DEBUG
    uart_str("UART Freq: "); uart_int(uart_response[1]); uart_nl();
#endif

    return uart_response[1];
}

u32 mbox_set_uart_freq(u32 uart_freq, u32 disable_turbo) {
    u32 uart_response[16];

    if (mbox_get_info(uart_response, 0x00038002, 12, (u32 []) {0x2,uart_freq,disable_turbo}, 8) < 8 ||
        uart_response[0] != 0x2) return 0;

#ifdef DEBUG
    uart_str("UART Freq: "); uart_int(uart_response[1]); uart_nl();
#endif

    return uart_response[1];
}

void mbox_get_firmware_version() {
#ifdef DEBUG
    uart_str("Get firmware version function\n");
#endif

    __attribute__((aligned(16))) u32 buffer[16];
    mbox_create_request(buffer, 0x00030002, 4, (u32 []){0x3}, 8);

    while ( (*REG_MBOX_WR_ST & MBOX_ST_FULL_MASK) );

    uintptr_t mail = (uintptr_t) buffer;

#ifdef DEBUG
    uart_str("Sending the following mail: "); uart_hex(mail); uart_nl();
    uart_str("Status Register: "); uart_hex(*REG_MBOX_WR_ST); uart_nl();
#endif

    mail &= ~(0xFU);
    mail |= 0x8;

    *REG_MBOX_WR = mail;

#ifdef DEBUG
    uart_str("Waiting for response: "); uart_hex(mail); uart_nl();
    uart_str("Status Register: "); uart_hex(*REG_MBOX_WR_ST); uart_nl();
#endif
    
    while ( (*REG_MBOX_RD_ST & MBOX_ST_EMPTY_MASK) );

#ifdef DEBUG
    uart_str("Message received\n");
    uart_str("Status Register: "); uart_hex(*REG_MBOX_RD_ST); uart_nl();
#endif

    u32 response[16];
    u32 res_size = mbox_get_response(response, buffer);

#ifdef DEBUG
    for (u32 i = 0; i < (res_size >> 2); i++) {
        uart_str("Response ");  uart_int(i);
        uart_str(" : ");        uart_int(response[i]);
        uart_nl();
    }
#endif
}
