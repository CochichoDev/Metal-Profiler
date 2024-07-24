/*
 * Developed by Diogo Cochicho
 */

#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <ctype.h>

#include "tty.h"
#include "api.h"

FD_TTY INIT_TTY(const char *path) {
    int fd;
    struct termios oldtio, newtio;
    /* 
      Open modem device for reading and writing and not as controlling tty
      because we don't want to get killed if linenoise sends CTRL-C.
    */
    fd = open(path, O_RDWR | O_NOCTTY ); 
    if (fd < 0) {
         perror(path); 
         exit(-1); 
     }

    tcgetattr(fd,&oldtio); /* save current serial port settings */
    bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

    /* 
      BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
      CRTSCTS : output hardware flow control (only used if the cable has
                all necessary lines. See sect. 7 of Serial-HOWTO)
      CS8     : 8n1 (8bit,no parity,1 stopbit)
      CLOCAL  : local connection, no modem contol
      CREAD   : enable receiving characters
    */
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
 
    /*
        IGNPAR  : ignore bytes with parity errors
        ICRNL   : map CR to NL (otherwise a CR input on the other computer
                    will not terminate input)
                    otherwise make device raw (no other input processing)
    */
    newtio.c_iflag = IGNPAR | ICRNL;
 
    /*
        Raw output.
    */
    newtio.c_oflag = 0;
 
    /*
        ICANON  : enable canonical input
        disable all echo functionality, and don't send signals to calling program
    */
    newtio.c_lflag = ICANON;
 
    /* 
        initialize all control characters 
        default values can be found in /usr/include/termios.h, and are given
        in the comments, but we don't need them here
    */
    newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
    newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    newtio.c_cc[VERASE]   = 0;     /* del */
    newtio.c_cc[VKILL]    = 0;     /* @ */
    newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
    newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
    newtio.c_cc[VSWTC]    = 0;     /* '\0' */
    newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
    newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    newtio.c_cc[VEOL]     = 0;     /* '\0' */
    newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
    newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    newtio.c_cc[VEOL2]    = 0;     /* '\0' */

    /* 
        now clean the modem line and activate the settings for the port
    */
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);

    return (FD_TTY) {fd, oldtio};
}

void READ_TTY_TO_RESULT(FD_TTY tty, RESULT *result, T_CHAR marker) {
    T_CHAR buf[256];
    
    volatile FLAG stop = 0;
    T_UINT read_bytes = 0;
    for (uint32_t idx = 0 ; stop == 0 ; idx++) {
        read_bytes = read(tty.fd,buf,255); 
        buf[read_bytes]='\0';          
        if (buf[0] == marker) stop=1;
        if (isdigit(buf[0])) {
            switch (result->TYPE) {
                case R_INT:
                    sscanf(buf, "%u", (T_INT *)(result->DATA+idx));
                case R_DOUBLE:
                    sscanf(buf, "%lf", (T_DOUBLE *)(result->DATA+idx));
            }
        }
        puts(buf);
    }
}

void CLOSE_TTY(FD_TTY tty) {
    /* restore the old port settings */
    tcsetattr(tty.fd,TCSANOW,&(tty.oldtio));
    close(tty.fd);
}
