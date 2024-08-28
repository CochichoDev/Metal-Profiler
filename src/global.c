/*
 * File: global.c
 * Definition and allocation of global state
 * Author: Diogo Cochicho
 */

#include <dlfcn.h>
#include <unistd.h>

#include "global.h"
#include "api.h"

T_STR OUTPUT_GRAPH_OPTIONS[NUM_OUTPUT_GRAPHS] = {   {"SCATTER"},
                                                    {"BARWERROR"}
                                                };
T_STR OUTPUT_DATA_OPTIONS[NUM_OUTPUT_DATA] =    {   {"RAW"},
                                                    {"DEGRADATION"}
                                                };

OUTPUT_LIST *OUTPUT_LIST_SELECTED = NULL;

ARCH SELECTED_ARCH;
ARCH_LIST AVAIL_ARCHS;

FCONFIG_LIST AVAIL_CONFIGS = {.config = {0}, .num = 0, .selected = -1};

T_VOID *MODULE_HANDLE = NULL;
CONFIG *MODULE_CONFIG = NULL;
CONFIG *INPUT_CONFIG = NULL;

T_VOID (*BUILD_PROJECT)(CONFIG *);
T_VOID (*INIT_BENCH)(T_VOID);
T_VOID (*RUN_BENCH)(RESULT *);
T_VOID (*EXIT_BENCH)(T_VOID);
