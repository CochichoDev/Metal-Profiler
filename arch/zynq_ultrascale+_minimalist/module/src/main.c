#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "api.h"
#include "results.h"
#include "t32_i.h"
#include "utils.h"
#include "global.h"
#include "tty.h"

pid_t T32_PID;
FD_TTY TTY;
CONFIG *CUR_CONFIG;

void BUILD_PROJECT(CONFIG *config) {
    CUR_CONFIG = config;
    puts("Makefile INFO");
    callMakefiles(config);
    puts("Info: Binaries compiled");
    puts("*****************************************************");


    const COMP *victim;
    T_UINT limit;
    if (GET_COMP_BY_IDX(CUR_CONFIG, 0, &victim) == -1) {
        fprintf(stderr, "Error: Victim core is not configured");
        return;
    }
    if (GET_PROP_BY_NAME(victim, "LIMIT", &limit) == -1) {
        fprintf(stderr, "Error: LIMIT propriety is not configured on the victim core");
        return;
    }
    REGISTER_OUTPUT(T_UINT, limit, "CYCLES");
    REGISTER_OUTPUT(T_UINT, limit, "L2_REFILLS");
}

void INIT_BENCH() {
    puts("Trace32 INFO");
    T32_PID = INIT_T32("/opt/t32/bin/pc_linux64/t32marm64-qt");
    if (T32_PID == -1) {
        perror("Error: Could not initialize TRACE32");
        exit(1);
    }
    puts("*****************************************************");
    puts("Info: Trace32 launched");
    puts("*****************************************************");

    TTY = INIT_TTY(TTY_PORT);
    puts("*****************************************************");
    puts("Info: TTY launched");
    puts("*****************************************************");
}

void RUN_BENCH(RESULT *results) {
    char script_query[256] = T32SCRIPT;

    for (size_t idx = 0 ; idx < NUM_CORES ; idx++) {
        if (GET_COMP_BY_IDX(CUR_CONFIG, idx, NULL) != -1)
            strcat(script_query, " \"TRUE\"");
        else
            strcat(script_query, " \"FALSE\"");
    }

    
    EX_T32_SCRIPT(script_query);
    
    READ_TO_RESULT(TTY.fd, 'F', results);
}

void EXIT_BENCH() {
    CLOSE_TTY(TTY);
    CLOSE_T32(T32_PID);

    puts("Info: Trace32 closed");
}
