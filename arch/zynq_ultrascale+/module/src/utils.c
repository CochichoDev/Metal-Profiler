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
        make_standalone = launchProcess("/bin/make", "make", "-C", PROJDIR"/Meta/psu_cortexa53_0/standalone_domain/bsp/psu_cortexa53_0/libsrc/standalone_v7_6/src", "EXTRA_COMPILER_FLAGS=-DCacheColoring", NULL);
    } else {
        make_standalone = launchProcess("/bin/make", "make", "-C", "Meta/psu_cortexa53_0/standalone_domain/bsp/psu_cortexa53_0/libsrc/standalone_v7_6/src", NULL);
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
            make_pids[i] = launchProcess("/bin/make", "make", "-C", PATH, "clean", "all", CFLAGS, NULL);
        }
    }
    
    for (uint8_t i = 0 ; i < NUM_CORES ; i++) {
        if (make_pids[i]) {
            waitpid(make_pids[i], NULL, 0);
        }
    }
    
}

void makeString(COMP *comp, STR_P CFLAGS) {
    memcpy(CFLAGS, "CFLAGS=", 8);
    char app_def[16] = "-D";
    char size_def[32] = "-DTARGET_SIZE=";
    char stride_def[32] = "-DSTRIDE=";
    char limit_def[32] = "-DLIMIT=";

    char *app; 
    GET_PROP_BY_NAME(comp, "APP", &app);
    strcat(app_def, app);

    INT size;
    char size_str[16];
    GET_PROP_BY_NAME(comp, "SIZE", &size);
    sprintf(size_str, "%d", size);
    strcat(size_def, size_str);

    INT stride;
    char stride_str[32];
    GET_PROP_BY_NAME(comp, "STRIDE", &stride);
    sprintf(stride_str, "%d", stride);
    strcat(stride_def, stride_str);

    INT limit;
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

pid_t launchProcess(const char *path, ...) {
    va_list va;
    const char *args[16];

    va_start(va, path);
    uint8_t idx;
    for (idx = 0; idx < sizeof(args)/sizeof(char*); idx++) {
        const char *arg = va_arg(va, char*);
        if (!arg) break;
        args[idx] = arg;
    }
    // The execv function needs to be ended with a NULL pointer
    args[idx] = NULL;
    va_end(va);


    pid_t child_process = fork();

    /*
     * In case the current process is the child
     * change the process image to the one demanded
     */
    if (child_process == -1) { 
        perror("Error: Could not fork the current process");
        exit(1);
    }
    if (!child_process) {
        //close(STDOUT_FILENO);
        execv(path, (char**) args);
    }

    return child_process;
}
