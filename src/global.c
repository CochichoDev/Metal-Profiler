/*
 * Developed by Diogo Cochicho
 */

#include <dlfcn.h>
#include <unistd.h>

#include "global.h"

T_STR OUTPUT_GRAPH_OPTIONS[NUM_OUTPUT_GRAPHS] = {   {"SCATTER"},
                                                    {"BARWERROR"}
                                                };
T_STR OUTPUT_DATA_OPTIONS[NUM_OUTPUT_DATA] =    {   {"CYCLES"},
                                                    {"DEGRADATION"}
                                                };
OUTPUT_LIST OUTPUT_LIST_SELECTED;

ARCH SELECTED_ARCH;
ARCH_LIST AVAIL_ARCHS;

FCONFIG_LIST AVAIL_CONFIGS;

T_VOID *MODULE_HANDLE;
CONFIG *MODULE_CONFIG;

T_VOID (*BUILD_PROJECT)(CONFIG *);
T_VOID (*INIT_BENCH)(T_VOID);
RESULT *(*RUN_BENCH)(T_VOID);
T_VOID (*EXIT_BENCH)(T_VOID);

