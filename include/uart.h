#pragma once

#include "common.h"
#include "global.h"

void init_uart(s8 port_number);
void close_uart();
void uart_send_byte(u8 byte);
u8 uart_receive_byte();
void uart_unlock();
void uart_log();
void uart_to_result(char imarker, char fmarker, RESULT *results);

void deployFirmware(const char *path);
