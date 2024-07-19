#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#include "global.h"
#include "utils.h"

void callMakefiles(void **config) {
    // config contains all the argument per order defined in data array in global.h
    CoreConfig *core_config;
    core_config = calloc(4, sizeof(CoreConfig));

    for (size_t i = 0 ; i < NUM_CORES ; i++) {
        if (*config)
            memcpy(core_config[i].application, (STR_P) *config, strlen((STR_P) *config)+1);
        config++;

        if (*config)
            core_config[i].target_size = *((INT *) *config);
        config++;
        
        if (*config)
            core_config[i].stride = *((INT *) *config);
        config++;

        if (i == 0) {
            core_config[i].limit = *((INT *) *config);
            config++;
        }
    
    }

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
        if (config[i] != NULL) {
            const char *CFLAGS = makeString(config[i]);
            puts(CFLAGS);
            char PATH[64];
            sprintf(PATH, MAKEFILE_PATH, i);
            make_pids[i] = launchProcess("/bin/make", "make", "-C", PATH, "clean", "all", CFLAGS, NULL);
            free((void *) CFLAGS);
        }
    }
    for (uint8_t i = 0 ; i < NUM_CORES ; i++) {
        if (make_pids[i]) {
            waitpid(make_pids[i], NULL, 0);
        }
    }
    free(core_config);
}

const char *makeString(CoreConfig *config) {
    char *CFLAGS = (char *) malloc(512);
    memcpy(CFLAGS, "CFLAGS=", 8);
    char app_def[16] = "-D";
    char size_def[32] = "-DTARGET_SIZE=";
    char stride_def[32] = "-DSTRIDE=";
    char limit_def[32] = "-DLIMIT=";
    char times_def[32] = "-DTIMES=";
    strcat(app_def, config->application);
    char times_str[4];
    sprintf(times_str, "%d", config->times);
    strcat(times_def, times_str);
    char size_str[16];
    sprintf(size_str, "%d", config->target_size);
    strcat(size_def, size_str);
    strcat(size_def, " ");
    char stride_str[32];
    sprintf(stride_str, "%d", config->stride);
    strcat(stride_def, stride_str);
    char limit_str[16];
    sprintf(limit_str, "%d", config->limit);
    strcat(limit_def, limit_str);
    
    strcat(CFLAGS, app_def);
    strcat(CFLAGS, " ");
    strcat(CFLAGS, times_def);
    strcat(CFLAGS, " ");
    strcat(CFLAGS, size_def);
    strcat(CFLAGS, " ");
    strcat(CFLAGS, stride_def);
    strcat(CFLAGS, " ");
    strcat(CFLAGS, limit_def);

    return CFLAGS;
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
