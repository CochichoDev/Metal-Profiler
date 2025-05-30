#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"
#include "elf_reader.h"
#include "global.h"
#include "uart.h"
#include "utils.h"

static s32 uart_fd = -1;
static s8 uart_lock = 0;

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

static u8 uart_handshake() {
    if (uart_fd == -1) {
        fprintf(stderr, "Error: UART was not correctly initialized\n");
        return 1;
    }
    do {
        uart_send_byte(0x1);
    } while (uart_receive_byte() != 0x1);

    uart_send_byte(0x0);

    printf("UART handshake completed\n");

    return 0;
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

    cfsetspeed(&tty, B115200);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // Minimum number of characters to read
    tty.c_cc[VTIME] = 10;            // 1.0 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= 0;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tcsetattr(uart_fd, TCSANOW, &tty);
    
    if (uart_handshake()) {
        fprintf(stderr, "Failed UART handshakre\n");
        exit(-1);
    }
}

void close_uart() {
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
    tcdrain(uart_fd);
}

u8 uart_receive_byte() {
    if (uart_fd == -1) {
        fprintf(stderr, "Error: UART was not correctly initialized\n");
        return -1;
    }
    u8 byte = 0;
    read(uart_fd, &byte, 1);
    return byte;
}

void uart_unlock() {
    if (uart_lock == 1) {
        while (uart_receive_byte() != 1);
    }
    uart_lock = 1;
}

void uart_log() {
    if (uart_fd == -1) {
        fprintf(stderr, "Error: UART was not correctly initialized\n");
        return;
    }
    
    char byte_buf = 0;
    do {
        read(uart_fd, &byte_buf, 1);
        putchar(byte_buf);
    } while (byte_buf);
    putchar('\n');
}

/* Needed because of canonical mode bug -> It should only return whenever a new line is detected */
static size_t read_line_from_tty(int fd, char *buf) {
    size_t idx = 0;
    char c_buf = 0;

    while (read(fd, &c_buf, 1)) {
        if (c_buf == '\n') break;

        buf[idx++] = c_buf;
    }
    buf[idx] = 0;
    return idx;
}


void uart_to_result(char imarker, char fmarker, RESULT *results) {
    assert(OUTPUT_LIST_SELECTED != NULL);
    assert(results != NULL);

    char buf[256] = { 0 };
    char *buf_ptr = buf;

    size_t numResults = 0;
    for (OUTPUT_LIST *out_ptr = OUTPUT_LIST_SELECTED; out_ptr != NULL; out_ptr = out_ptr->NEXT, ++numResults);
    
    #ifdef DEBUG
    printf("Number of results: %lu\n", numResults);
    #endif

    volatile T_FLAG stop = 0;
    uint32_t read_bytes = 0;

    do {
        read_bytes = read_line_from_tty(uart_fd, buf); 
        if (read_bytes < 1) continue;
        puts(buf);
    } while(buf[0] != '$');

    for (uint32_t idx = 0, total = 0 ; stop == 0 ; ) {
        read_bytes = read_line_from_tty(uart_fd, buf);
        if (read_bytes < 1) continue;
        puts(buf);
        if (buf[0] == fmarker) {
            stop=1;
        }
        buf_ptr = buf;
        while ( *(buf_ptr = get_result_ptr(buf_ptr, imarker)) != 0 ) {
            #ifdef DEBUG
            printf("Result in index %lu has a size of %lu\n", total % numResults,  results[total % numResults].ARRAY.SIZE);
            #endif
            if (idx >= results[total % numResults].ARRAY.SIZE)
                goto CONTINUE;
            switch (results[total % numResults].ARRAY.TYPE) {
                case G_INT:
                    sscanf(buf_ptr, "%u", ((T_UINT *)results[total % numResults].ARRAY.DATA)+idx);
                    //printf("%u\n", *(((T_UINT *)results[total % numResults].ARRAY.DATA)+idx));
                    break;
                case G_UINT:
                    sscanf(buf_ptr, "%u", ((T_UINT *)results[total % numResults].ARRAY.DATA)+idx);
                    //printf("%u\n", *(((T_UINT *)results[total % numResults].ARRAY.DATA)+idx));
                    break;
                case G_DOUBLE:
                    sscanf(buf_ptr, "%lf", ((T_DOUBLE *)results[total % numResults].ARRAY.DATA)+idx);
                    //printf("%f\n", *(((T_DOUBLE *)results[total % numResults].ARRAY.DATA)+idx));
                default:
                    break;
            } 
        CONTINUE:
            total++;
            idx = total/numResults;
        }
    }
}

