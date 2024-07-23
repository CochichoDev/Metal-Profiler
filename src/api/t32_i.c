#include <unistd.h>
#include <stdio.h>

#include "api/t32_i.h"
#include "api/t32.h"

T_INT INIT_T32_CONN(const char *node, const char *port) {
    if (T32_Config("NODE=", node) != T32_OK) {
        perror("Error: Could not configure node on T32 connection\n");
        return 1;
    }
    if (T32_Config("PORT=", port) != T32_OK) {
        perror("Error: Could not configure port on T32 connection\n");
        return 1;
    }

    while (1) {
        if (T32_Init() == T32_OK) {
            if (T32_Attach(T32_DEV_ICD) == T32_OK) {
                fprintf(stderr, "Info: Connection to Trace32 established\n");
                break;
            }
        }
        fprintf(stderr, "Error: Could not initilize T32 connection\n");
        fprintf(stderr, "Info: Retrying connection...\n");
        sleep(5);
    }

    fprintf(stderr, "All T32 connection issues were sucessful");
    return 0;
}

T_INT CLOSE_T32_CONN() {
    if (T32_Exit() != T32_OK) {
        perror("Error: Could not close T32 connection\n");
        return 1;
    }
    return 0;
}

T_INT EX_T32_SCRIPT(const char *scriptname) {
    if (T32_Cmd_f("DO %s", scriptname) != T32_OK) {
        fprintf(stderr, "Error: Could not execute %s\n", scriptname);
        return 1;
    }
    return 0;
}
