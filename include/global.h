/*
 * Developed by Diogo Cochicho
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "api/api.h"

#define ARCHS_PATH "arch/"
#define ARCH_CONFIG "archs.txt"

#define DEVICE "/dev/ttyUSB0"

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
    uint8_t mode;               // ENEMY:   0 ; VICTIM: 1
    char application[32];
    uint8_t times;
    uint32_t target_size;
    uint16_t stride;
    uint32_t limit;
} CoreConfig;

/*
 * Generic type of Results
 */
#define RESULTS(T)              \
    struct {                    \
        char    name[64];       \
        size_t  num_cycles;     \
        T*      cycles;         \
        T       median;         \
        T       average;        \
        T       max, min;       \
        T       std_deviation;  \
    }




#define initializeResults(T, results, num_cycles, name) \
    T##_initializeResults(results, num_cycles, name);

#define destroyResults(T, results) \
    T##_destroyResults(results);

/*
 * Global variable declaration
 */
extern ARCH         SELECTED_ARCH;
extern ARCH_LIST    AVAIL_ARCHS;

extern void         *MODULE_HANDLE;
extern CONFIG       *MODULE_CONFIG;

extern void         (*LOAD_CONFIG)(void **);

/*
 * Function declaration
 */
void initializeFramework();
void getInitialConfig(const char *input, CoreConfig **config);
const char *makeString(CoreConfig *config);

void uint64_t_initializeResults(void *results_ptr, uint64_t num_cycles, const char *name);
void double_initializeResults(void *results_ptr, uint64_t num_cycles, const char *name);
void uint64_t_destroyResults(void *results_ptr);
void double_destroyResults(void *results_ptr);
