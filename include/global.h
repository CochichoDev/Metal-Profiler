#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "state.h"

#define TTY_PORT    "/dev/ttyUSB0"

#define ARCHS_PATH "arch/"
#define ARCH_CONFIG "archs.txt"

#define T32SCRIPT "launch_bench.cmm"
#define XSCTSCRIPT "launch_bench.tcl"

#define T32EXECUTABLE "/opt/t32/bin/pc_linux64/t32marm64-qt"
//#define XSCTEXECUTABLE "/tools/Xilinx/Vitis/2021.2/bin/xsct"
#define XSCTEXECUTABLE "/home/cochicho/Vivado_Lab/2024.1/bin/xsdb"
#define MAX_ARCHS 256

#define SYSTEM_COMP_ID 0

/**** DEGRADATION PARAMETERS ****/
#define IGNORE_LIMIT 10
#define TARGET_SIZE_DIVISOR 0.9
//#define TARGET_SIZE_DIVISOR 0.99

/********************************/
/**** OPTIMIZATION PARAMETERS ****/
#define NUM_ISO_RESULTS 1
#define WEIGHT1         9
#define WEIGHT2         1
// FOR 2 WEIGHT OPTIMIZATION
#define STD_DEV_MAX     1.0f
#define MIN_DEG     0.7f
#define MAX_DEG     2.0f
#define MIN_OVERHEAD     0.9f
#define MAX_OVERHEAD     10.0f
/*********************************/

#define STR(s) #s
#define STR_VALUE(s) STR(s)
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

extern void     (*DEPLOY)(const char *);
extern void     (*BUILD_PROJECT)(CONFIG *);
extern void     (*INIT_BENCH)(void);
extern void     (*RUN_BENCH)(RESULT *);
extern void     (*EXIT_BENCH)(T_VOID);
#endif

