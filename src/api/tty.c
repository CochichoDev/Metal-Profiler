/*
 * File: tty.c
 * TTY related functionality provided by the API
 * Author: Diogo Cochicho
 */

#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>

#include "tty.h"
#include "types.h"
#include "state.h"
#include "global.h"

static FD_TTY TTY;

static size_t read_line_from_tty(int fd, char *buf);

T_ERROR INIT_TTY(const char *path) {
    int fd;
    struct termios oldtio, newtio;

    fd = open(path, O_RDWR | O_NOCTTY ); 
    if (fd < 0) {
         perror(path); 
         return -1;
     }

    tcgetattr(fd,&oldtio); /* save current serial port settings */
    memcpy(&newtio, &oldtio, sizeof(struct termios));

    cfsetispeed(&newtio, BAUDRATE);
    cfsetospeed(&newtio, BAUDRATE);

    newtio.c_lflag |= ICANON;
    newtio.c_lflag &= ~ECHO;
    newtio.c_lflag &= ~ECHONL;

    newtio.c_cc[VEOL] = '\n';   
    newtio.c_cc[VEOL2] = '\r';

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);

    TTY.fd = fd;
    TTY.oldtio = oldtio;

    return 0;
}

void CLOSE_TTY() {
    /* restore the old port settings */
    tcsetattr(TTY.fd,TCSANOW,&(TTY.oldtio));
    close(TTY.fd);

    bzero(&TTY, sizeof(FD_TTY));
}

/************** RESULT MANIPULATION ****************/
void TTY_TO_RESULT(T_CHAR imarker, T_CHAR fmarker, RESULT *results) {
    assert(OUTPUT_LIST_SELECTED != NULL);
    assert(results != NULL);

    T_CHAR buf[256] = { 0 };

    size_t numResults = 0;
    for (OUTPUT_LIST *out_ptr = OUTPUT_LIST_SELECTED; out_ptr != NULL; out_ptr = out_ptr->NEXT, ++numResults);
    
    #ifdef DEBUG
    printf("Number of results: %lu\n", numResults);
    #endif

    volatile T_FLAG stop = 0;
    T_UINT read_bytes = 0;

    do {
        read_bytes = read(TTY.fd,buf,1); 
        if (read_bytes < 1) continue;
    } while(buf[0] != '$');

    for (uint32_t idx = 0, total = 0 ; stop == 0 ; ) {
        read_bytes = read_line_from_tty(TTY.fd, buf);
        if (read_bytes < 1) continue;
        puts(buf);
        if (buf[0] == fmarker) {
            stop=1;
        }
        if (read_bytes > 1 && buf[0] == imarker && isdigit(buf[1])) {
            #ifdef DEBUG
            printf("Result in index %lu has a size of %lu\n", total % numResults,  results[total % numResults].ARRAY.SIZE);
            #endif
            if (idx >= results[total % numResults].ARRAY.SIZE)
                goto CONTINUE;
            switch (results[total % numResults].ARRAY.TYPE) {
                case G_INT:
                    sscanf(buf+1, "%u", ((T_UINT *)results[total % numResults].ARRAY.DATA)+idx);
                    //printf("%u\n", *(((T_UINT *)results[total % numResults].ARRAY.DATA)+idx));
                    break;
                case G_UINT:
                    sscanf(buf+1, "%u", ((T_UINT *)results[total % numResults].ARRAY.DATA)+idx);
                    //printf("%u\n", *(((T_UINT *)results[total % numResults].ARRAY.DATA)+idx));
                    break;
                case G_DOUBLE:
                    sscanf(buf+1, "%lf", ((T_DOUBLE *)results[total % numResults].ARRAY.DATA)+idx);
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
