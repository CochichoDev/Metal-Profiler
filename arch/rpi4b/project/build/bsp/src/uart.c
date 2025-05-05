#include "common.h"
#include "uart_internals.h"
#include "uart.h"
#include "utils.h"

void uart_str(char *str) {
    for (; *str != 0; str++) outbyte(*str);
}

void uart_int(s64 num) {
    char nbuf[32];
    itos(num, nbuf);
    uart_str(nbuf);
}

void uart_hex(u64 num) {
    char nbuf[32];
    i16tos(num, nbuf);
    uart_str(nbuf);
}

void uart_nl() {
    outbyte('\n');
}

void get_uart_input(char input[], u32 max_size) {
    char *ptr = input;
    while ((ptr - input) < max_size) {
        *ptr = inbyte();
        if (*ptr == '\r' || *ptr == '\n') break;
        ++ptr;
    }
}
