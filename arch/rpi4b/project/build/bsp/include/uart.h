#pragma once

#include "common.h"

void uart_str(char *str);
void uart_int(u64 num);
void uart_hex(u64 num);
void uart_nl();
void get_uart_input(char input[], u32 max_size);
