#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "global.h"
#include "xsct_i.h"

static pid_t XSCT_PID;

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
                query[num_selected+1] = calloc(1, sizeof(char[32])*(2+num_selected));
                sprintf(query[num_selected+1], "Core%ld", i);
            }
        }
    }
    query[num_selected+2] = NULL;


    XSCT_PID = RUN_PROCESS_IMAGE_V(NULL, path, query);

    for (size_t idx = 2; idx < 7; idx++)
        free(query[idx]);

    return 0;
}

T_VOID CLOSE_XSCT() {
    waitpid(XSCT_PID, NULL, 0);
}
