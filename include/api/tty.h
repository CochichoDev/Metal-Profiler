/*
 * Developed by Diogo Cochicho
 */

#pragma once
#include <termio.h>

#include "api.h"

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
} FD_TTY;

FD_TTY INIT_TTY(const char *path);
void CLOSE_TTY(FD_TTY fd);
