#pragma once

#include "common.h"

void uart_str(char *str);
void uart_int(s64 num);
void uart_hex(u64 num);
void uart_nl();
void get_uart_input(char input[], u32 max_size);

#define lock_mutex(LOCK)\
    __asm__(" \
            %=: ldxr    w0, %0      \n\
                cbnz    w0, %=b     \n\
                mov     w0, #1      \n\
                stxr    w1, w0, %0  \n\
                cbnz    w1, %=b     \n\
                dmb     sy"         \
                : : "m" (*LOCK)      \
                : "x0", "x1");

#define unlock_mutex(LOCK)          \
    __asm__("\
                dmb    sy           \n\
                str    wzr, %0"     \
                : : "m" (*LOCK)      \
                : "x0");

