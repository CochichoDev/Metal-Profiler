/*
 * Developed by Diogo Cochicho
 */

#pragma once
#include <termio.h>

/* baudrate settings are defined in <asm/termbits.h>, which is
included by <termios.h> */
#define BAUDRATE B115200
/* change this definition for the correct port */
#define _POSIX_SOURCE 1 /* POSIX compliant source */

/*
 * Type declarations
 */
typedef struct s_ttyFD {
    int fd;
    struct termios oldtio;
} ttyFD;

ttyFD openUltrascaleTTY(const char *path);
void closeUltrascaleTTY(ttyFD fd);
