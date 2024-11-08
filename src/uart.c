#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"

static s32 uart_fd = -1;



static err scan_tty_ports() {
    DIR *dev = opendir("/dev");
    struct dirent *entry;
    while ((entry = readdir(dev)) != NULL) {
        if (!strncmp(entry->d_name, "ttyUSB", 6)) {
            return entry->d_name[6] - '0';
        }
    }
    return -1;
}

/*
 * Initializes the UART
 * port_number: If -1 then tries to discover automatically
 */
void init_uart(s8 port_number) {
    u8 port = (port_number == -1) ? scan_tty_ports() : port_number;

    char path[13] = "/dev/ttyUSB";
    path[11] = port + '0';
    path[12] = 0;

    uart_fd = open(path, O_RDWR);
}

void close_uart(s8 port_number) {
    if (uart_fd != -1) {
        close(uart_fd);
    }
}

void uart_send_byte(u8 byte) {
    if (uart_fd == -1) {
        fprintf(stderr, "Error: UART was not correctly initialized\n");
        return;
    }
    write(uart_fd, &byte, 1);
}
