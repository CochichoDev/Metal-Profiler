#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "tty.h"
#include "results.h"
#include "types.h"
#include "state.h"

#define TTY_PORT    "/dev/ttyUSB1"

#define ARCHS_PATH "arch/"
#define ARCH_CONFIG "archs.txt"

#define T32SCRIPT "launch_bench.cmm"


#define MAX_ARCHS 256

#define REP_EXP 20
#define IGNORE_LIMIT 10

#define SYSTEM_COMP_ID 0

#define USAGE_ERROR() \
    {                                                       \
    perror("Usage: ./autometalbench -i INPUT -o OUTPUT");   \
    exit(1);                                                \
    }


extern T_STR        OUTPUT_GRAPH_OPTIONS[NUM_OUTPUT_GRAPHS];
extern T_STR        OUTPUT_DATA_OPTIONS[NUM_OUTPUT_DATA];

extern OUTPUT_LIST  *OUTPUT_LIST_SELECTED;

extern ARCH         SELECTED_ARCH;
extern ARCH_LIST    AVAIL_ARCHS;

extern FCONFIG_LIST        AVAIL_CONFIGS;

extern T_VOID       *MODULE_HANDLE;
extern CONFIG       *MODULE_CONFIG;
extern CONFIG       *INPUT_CONFIG;

extern T_VOID       (*BUILD_PROJECT)(CONFIG *);
extern T_VOID       (*INIT_BENCH)(T_VOID);
extern T_VOID       (*RUN_BENCH)(RESULT *);
extern T_VOID       (*EXIT_BENCH)(T_VOID);
#endif

