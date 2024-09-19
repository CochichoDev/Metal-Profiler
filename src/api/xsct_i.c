#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "global.h"
#include "xsct_i.h"

static pid_t XSCT_PID;

T_INT INIT_XSCT(char *scriptname) {
    char path[256] = XSCTEXECUTABLE;

    T_PSTR slash_marker = path;
    T_PSTR last_slash = path;
    while (*slash_marker != '\0') {
        if (*slash_marker == '/')
            last_slash = slash_marker;
        slash_marker++;
    }
    last_slash++;

    char *query[5] = {last_slash, scriptname};


    query[2] = calloc(1, sizeof(char[32]));
    strcpy(query[2], "FSBL");
    query[3] = calloc(1, sizeof(char[32]));
    strcpy(query[3], "LOADER");
    query[4] = NULL;

    XSCT_PID = RUN_PROCESS_IMAGE_V(NULL, path, query);

    int xsct_status;
    waitpid(XSCT_PID, &xsct_status, 0);

    return xsct_status;
}

T_INT EX_XSCT_SCRIPT(const char *scriptname, size_t num_cores, T_FLAG core_state[]) {
    char path[256] = XSCTEXECUTABLE;

    T_PSTR slash_marker = path;
    T_PSTR last_slash = path;
    while (*slash_marker != '\0') {
        if (*slash_marker == '/')
            last_slash = slash_marker;
        slash_marker++;
    }
    last_slash++;

    char *query[7] = {last_slash, scriptname};

    size_t num_selected = 0;
    for (size_t i = num_cores ; i > 0 ; i--) {
        if (core_state != NULL) {
            if (core_state[i-1]) {
                num_selected++;
                query[num_selected+1] = calloc(1, sizeof(char[32]));
                sprintf(query[num_selected+1], "Core%ld", i);
            }
        }
    }
    query[num_selected+2] = NULL;


    XSCT_PID = RUN_PROCESS_IMAGE_V(NULL, path, query);

    for (size_t idx = 2; idx < 2+num_selected; idx++)
        free(query[idx]);

    return 0;
}

T_VOID CLOSE_XSCT() {
    return;
}
