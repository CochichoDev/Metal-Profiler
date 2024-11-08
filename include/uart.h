#pragma once

#include "common.h"

void init_uart(s8 port_number);
void close_uart();
void uart_send_byte(u8 byte);
