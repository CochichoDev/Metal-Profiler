#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "api.h"
#include "global.h"
#include "tty.h"

pid_t T32_PID;
FD_TTY TTY;
RESULT results;
CONFIG *CUR_CONFIG;

enum operation {
    FLC,
    LLC,
    DRAM,
    FLC_MC,
    LLC_MC,
    DRAM_MC
} OPER = 0; 

void BUILD_PROJECT(CONFIG *config) {
    CUR_CONFIG = config;
}

void INIT_BENCH() {
    COMP *comp0 = NULL, *comp1 = NULL;
    GET_COMP_BY_IDX(CUR_CONFIG, 0, &comp0);
    GET_COMP_BY_IDX(CUR_CONFIG, 1, &comp1);

    T_PSTR app_name;
    GET_PROP_BY_NAME(comp0, "APP", &app_name);
    T_UINT limit;
    GET_PROP_BY_NAME(comp0, "LIMIT", &limit);

    INITIALIZE_RESULTS(T_UINT, &results, limit, "Cycles");

    // If an additional component is verified, start with MCP values
    OPER = 0;
    if (comp1 != NULL) {
        OPER += 3;
    }

    if (!strcmp(app_name, "FLC")) {
        OPER += 0;
    } else if (!strcmp(app_name, "LLC")) {
        OPER += 1;
    } else {
        OPER += 2;
    }

    if (GET_PROP_BY_NAME(comp0, "MITIGATION", NULL) != -1) {
        OPER = 5;
    }
}

RESULT *RUN_BENCH() {
    for (size_t idx = 0; idx < results.ARRAY.SIZE; idx++) {
        switch (OPER) {
            case FLC:
                ((T_UINT *)results.ARRAY.DATA)[idx] = 100 + (rand() % 10);
                break;
            case FLC_MC:
                ((T_UINT *)results.ARRAY.DATA)[idx] = 100 + (rand() % 10);
                break;
            case LLC:
                ((T_UINT *)results.ARRAY.DATA)[idx] = 500 + (rand() % 50);
                break;
            case DRAM:
                ((T_UINT *)results.ARRAY.DATA)[idx] = 3000 + (rand() % 500);
                break;
            case LLC_MC:
                ((T_UINT *)results.ARRAY.DATA)[idx] = 1000 + (rand() % 150);
                break;
            case DRAM_MC:
                ((T_UINT *)results.ARRAY.DATA)[idx] = 10000 + (rand() % 1000);
        }
    }



    return &results;
}

void EXIT_BENCH() {
    DESTROY_RESULTS(T_UINT, &results);
    OPER = 0;
}
