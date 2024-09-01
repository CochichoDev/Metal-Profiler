#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#include "api.h"
#include "global.h"
#include "utils.h"

void makeString(COMP *comp, T_PSTR CFLAGS) {
    memcpy(CFLAGS, "AFLAGS=", 8);
    char app_def[16] = "-D";
    char size_def[32] = "-DTARGET_SIZE=";
    char stride_def[32] = "-DSTRIDE=";
    char limit_def[32] = "-DLIMIT=";
    char bandwith_def[32] = "-DMEMBANDWIDTH";

    char *app; 
    GET_PROP_BY_NAME(comp, "APP", &app);
    strcat(app_def, app);

    T_INT size;
    char size_str[16];
    GET_PROP_BY_NAME(comp, "SIZE", &size);
    sprintf(size_str, "%d", size);
    strcat(size_def, size_str);

    T_INT stride;
    char stride_str[32];
    GET_PROP_BY_NAME(comp, "STRIDE", &stride);
    sprintf(stride_str, "%d", stride);
    strcat(stride_def, stride_str);

    T_INT limit;
    if (GET_PROP_BY_NAME(comp, "LIMIT", &limit) != -1) {
        char limit_str[16];
        sprintf(limit_str, "%d", limit);
        strcat(limit_def, limit_str);
        
        strcat(CFLAGS, limit_def);
        strcat(CFLAGS, " ");
    }
    strcat(CFLAGS, app_def);
    strcat(CFLAGS, " ");
    strcat(CFLAGS, size_def);
    strcat(CFLAGS, " ");
    strcat(CFLAGS, stride_def);

    if (GET_PROP_BY_NAME(comp, "MEMBANDWIDTH" , NULL) != -1) {
        strcat(CFLAGS, " ");
        strcat(CFLAGS, bandwith_def);
    }
    strcat(CFLAGS, " ");
}

static int addCacheColoring(const COMP *comp, T_PSTR CFLAGS) {
    if (GET_PROP_BY_NAME(comp, "CACHECOLORING", NULL) != -1) {
        strcat(CFLAGS, "-DCACHECOLORING");
        return 1;
    }
    return 0;
}

static void addMemBandwidthArgs(COMP *comp, T_PSTR CFLAGS) {
    char init_budget_def[64] = "-DINIT_BUDGET=";
    char period_def[64] = "-DPERIOD=";
    char replenishment_def[64] = "-DREPLENISHMENT=";

    T_INT budget_int;
    T_INT period_int;
    T_INT replenishment_int;
    char init_budget_value[32];
    char period_value[32];
    char replenishment_value[32];

    if (GET_PROP_BY_NAME(comp, "BUDGET", &budget_int) != -1) {
        strcat(CFLAGS, " ");
        strcat(CFLAGS, init_budget_def);
        sprintf(init_budget_value, "%d", budget_int);
        strcat(CFLAGS, init_budget_value);
    }
    if (GET_PROP_BY_NAME(comp, "PERIOD", &period_int) != -1) {
        strcat(CFLAGS, " ");
        strcat(CFLAGS, period_def);
        sprintf(period_value, "%d", period_int);
        strcat(CFLAGS, period_value);
    }
    if (GET_PROP_BY_NAME(comp, "REPLENISHMENT", &replenishment_int) != -1) {
        strcat(CFLAGS, " ");
        strcat(CFLAGS, replenishment_def);
        sprintf(replenishment_value, "%d", replenishment_int);
        strcat(CFLAGS, replenishment_value);
    }
    strcat(CFLAGS, " ");
}

void callMakefiles(CONFIG *config) {
    pid_t make_pids[NUM_CORES] = { 0 };
    pid_t make_standalone;

    const COMP *victim_comp;
    if (GET_COMP_BY_IDX(config, config->VICTIM_ID, &victim_comp) == -1) {
        fprintf(stderr, "Error: Zynq Ultrascale+ received a config without the victim component\n");
        exit(-1);
    }

    char CFLAGS_STANDALONE[512] = "CFLAGS=";
    addCacheColoring(victim_comp, CFLAGS_STANDALONE);
    puts(CFLAGS_STANDALONE);
    make_standalone = RUN_PROCESS_IMAGE(NULL, "/bin/make", "make", "-C", PROJDIR"/bsp/", "clean", NULL);
    waitpid(make_standalone, NULL, 0);
    make_standalone = RUN_PROCESS_IMAGE(NULL, "/bin/make", "make", "-C", PROJDIR"/bsp/", CFLAGS_STANDALONE, NULL);
    waitpid(make_standalone, NULL, 0);

    const COMP *mem_monitoring = NULL;
    if (config->NUM == 5)
        GET_COMP_BY_IDX(config, 4, &mem_monitoring);

    for (uint8_t i = 0 ; i < NUM_CORES ; i++) {
        const COMP *core_ptr;
        if (GET_COMP_BY_IDX(config, i, &core_ptr) != -1) {
            char AFLAGS[1024];
            makeString((COMP*) core_ptr, AFLAGS);

            if (mem_monitoring != NULL) {
                addMemBandwidthArgs((COMP *) mem_monitoring, AFLAGS);
            }

            addCacheColoring(core_ptr, AFLAGS);

            puts(AFLAGS);

            char PATH[64];
            sprintf(PATH, MAKEFILE_PATH, i);
            make_pids[i] = RUN_PROCESS_IMAGE(NULL, "/bin/make", "make", "-C", PATH, "clean", "all", AFLAGS, NULL);
        }
    }
    
    for (uint8_t i = 0 ; i < NUM_CORES ; i++) {
        if (make_pids[i]) {
            waitpid(make_pids[i], NULL, 0);
        }
    }
}

