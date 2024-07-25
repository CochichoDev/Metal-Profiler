/*
 * Developed by Diogo Cochicho
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "api/api.h"

#define ARCHS_PATH "arch/"
#define ARCH_CONFIG "archs.txt"

#define MAKEFILE0 "Core0/Debug"
#define MAKEFILE1 "Core1/Debug"

#define T32SCRIPT "launch_bench.cmm"

#define FALSE 0
#define TRUE 1

#define MAX_ARCHS 256

#define REP_EXP 20
#define NUM_CORES 4
#define IGNORE_LIMIT 10

#define USAGE_ERROR() \
    {                                                       \
    perror("Usage: ./autometalbench -i INPUT -o OUTPUT");   \
    exit(1);                                                \
    }

/*
 * Type declarations
 */
typedef struct {
    char name[128];
    char path[256];
} ARCH;

typedef struct {
    ARCH arch[64];
    size_t num;
} ARCH_LIST;

typedef struct {
    char name[128];
    char path[256];
} FCONFIG;

typedef struct {
    FCONFIG config[128];
    size_t num;
} FCONFIG_LIST;

#define NUM_OUTPUT_GRAPHS 2
#define NUM_OUTPUT_DATA 2
typedef struct {
    T_STR NAME;
    enum {
        SCATTER,
        BARWERROR
    } GRAPH_TYPE;
    enum {
        CYCLES,
        DEGRADATION
    } DATA_TYPE;
} OUTPUT;

typedef struct s_OUTPUT_LIST {
    OUTPUT *OUT;
    struct s_OUTPUT_LIST *NEXT;
} OUTPUT_LIST;

/*
 * Global variable declaration
 */
extern T_STR        OUTPUT_GRAPH_OPTIONS[NUM_OUTPUT_GRAPHS];
extern T_STR        OUTPUT_DATA_OPTIONS[NUM_OUTPUT_DATA];
extern OUTPUT_LIST  OUTS;

extern ARCH         SELECTED_ARCH;
extern ARCH_LIST    AVAIL_ARCHS;

extern FCONFIG_LIST        AVAIL_CONFIGS;

extern T_VOID       *MODULE_HANDLE;
extern CONFIG       *MODULE_CONFIG;

extern T_VOID       (*BUILD_PROJECT)(CONFIG *);
extern T_VOID       (*INIT_BENCH)(T_VOID);
extern RESULT       *(*RUN_BENCH)(T_VOID);
extern T_VOID       (*EXIT_BENCH)(T_VOID);

