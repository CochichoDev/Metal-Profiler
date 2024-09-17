#ifndef TTY_H
#define TTY_H

#include <termio.h>

#include "api.h"

/* baudrate settings are defined in <asm/termbits.h>, which is
included by <termios.h> */
#define BAUDRATE B115200
/* change this definition for the correct port */
#define _POSIX_SOURCE 1 /* POSIX compliant source */

/************** TYPES DEFINITION ****************/
typedef struct s_ttyFD {
    int fd;
    struct termios oldtio;
} FD_TTY;

T_ERROR INIT_TTY(const char *path);
T_VOID CLOSE_TTY();

/************** RESULT MANIPULATION ****************/
void TTY_TO_RESULT(T_CHAR imarker, T_CHAR fmarker, RESULT *results);

#endif
