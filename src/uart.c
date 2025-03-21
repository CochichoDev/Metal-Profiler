#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"
#include "elf_reader.h"

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
    if (uart_fd != -1) return;

    struct termios tty;
    u8 port = (port_number == -1) ? scan_tty_ports() : port_number;

    char path[13] = "/dev/ttyUSB";
    path[11] = port + '0';
    path[12] = 0;

    uart_fd = open(path, O_RDWR);
    if (uart_fd == -1) {
        fprintf(stderr, "Error: Not able to open %s\n", path);
        exit(-1);
    }
    tcgetattr(uart_fd, &tty);

    cfsetspeed(&tty, B230400);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
                                    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= 0;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tcsetattr(uart_fd, TCSANOW, &tty);
}

void close_uart(s8 port_number) {
    if (uart_fd != -1) {
        close(uart_fd);
    }
    uart_fd = -1;
}

void uart_send_byte(u8 byte) {
    if (uart_fd == -1) {
        fprintf(stderr, "Error: UART was not correctly initialized\n");
        return;
    }
    write(uart_fd, &byte, 1);
}

