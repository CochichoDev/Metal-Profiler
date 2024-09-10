#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "results.h"
#include "state.h"
#include "tty.h"
#include "t32_i.h"
#include "global.h"
#include "utils.h"

static CONFIG *CUR_CFG;

void default_BUILD_PROJECT(CONFIG *config) {
    CUR_CFG = config;
    puts("Makefile INFO");
    CALL_MAKEFILES(config);
    puts("Info: Binaries compiled");
    puts("*****************************************************");


    const COMP *victim;
    T_UINT limit;
    if (GET_COMP_BY_ID(config, config->VICTIM_ID, &victim) == -1) {
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

void default_INIT_BENCH() {
    puts("Trace32 INFO");
    pid_t T32_PID = INIT_T32("/opt/t32/bin/pc_linux64/t32marm64-qt");
    if (T32_PID == -1) {
        perror("Error: Could not initialize TRACE32");
        exit(1);
    }
    puts("*****************************************************");
    puts("Info: Trace32 launched");
    puts("*****************************************************");

    INIT_TTY(TTY_PORT);
    puts("*****************************************************");
    puts("Info: TTY launched");
    puts("*****************************************************");
}

void default_RUN_BENCH(RESULT *results) {
    char script_query[256];
    strcpy(script_query, SELECTED_ARCH.path);
    strcat(script_query, "/project/"T32SCRIPT);

    T_FLAG *core_state = alloca(sizeof(T_FLAG) * SELECTED_ARCH.desc.NUM_CORES);

    for (size_t idx = 1 ; idx <= SELECTED_ARCH.desc.NUM_CORES ; idx++) {
        core_state[idx-1] = (GET_COMP_BY_ID(CUR_CFG, idx, NULL) != -1) ? 1 : 0;
    }
    
    EX_T32_SCRIPT(script_query, SELECTED_ARCH.desc.NUM_CORES, core_state);
    
    TTY_TO_RESULT('F', results);
}

void default_EXIT_BENCH() {
    CLOSE_TTY();
    CLOSE_T32();

    puts("Info: Trace32 closed");
}
