#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#include "api.h"
#include "global.h"
#include "utils.h"

void callMakefiles(CONFIG *config) {
    FLAG cacheColoring = 1;
    pid_t make_pids[NUM_CORES] = { 0 };
    pid_t make_standalone;

    if (cacheColoring) {
        make_standalone = RUN_PROCESS_IMAGE(NULL, "/bin/make", "make", "-C", PROJDIR"/Meta/psu_cortexa53_0/standalone_domain/bsp/psu_cortexa53_0/libsrc/standalone_v7_6/src", "EXTRA_COMPILER_FLAGS=-DCacheColoring", NULL);
    } else {
        make_standalone = RUN_PROCESS_IMAGE(NULL, "/bin/make", "make", "-C", "Meta/psu_cortexa53_0/standalone_domain/bsp/psu_cortexa53_0/libsrc/standalone_v7_6/src", NULL);
    }
    waitpid(make_standalone, NULL, 0);

    for (uint8_t i = 0 ; i < NUM_CORES ; i++) {
        COMP *core_ptr;
        if (GET_COMP_BY_IDX(config, i, &core_ptr) != -1) {
            char CFLAGS[512];
            makeString(core_ptr, CFLAGS);
            puts(CFLAGS);
            char PATH[64];
            sprintf(PATH, MAKEFILE_PATH, i);
            make_pids[i] = RUN_PROCESS_IMAGE(NULL, "/bin/make", "make", "-C", PATH, "clean", "all", CFLAGS, NULL);
        }
    }
    
    for (uint8_t i = 0 ; i < NUM_CORES ; i++) {
        if (make_pids[i]) {
            waitpid(make_pids[i], NULL, 0);
        }
    }
}

void makeString(COMP *comp, T_PSTR CFLAGS) {
    memcpy(CFLAGS, "CFLAGS=", 8);
    char app_def[16] = "-D";
    char size_def[32] = "-DTARGET_SIZE=";
    char stride_def[32] = "-DSTRIDE=";
    char limit_def[32] = "-DLIMIT=";

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
}

