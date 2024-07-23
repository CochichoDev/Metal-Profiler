/*
 * Developed by Diogo Cochicho
 */

#include <dlfcn.h>
#include <unistd.h>

#include "global.h"

ARCH SELECTED_ARCH;
ARCH_LIST AVAIL_ARCHS;

void *MODULE_HANDLE;
CONFIG *MODULE_CONFIG;

void (*BUILD_PROJECT)(CONFIG *);
void (*INIT_BENCH)(void);
void (*RUN_BENCH)(void);
void (*EXIT_BENCH)(void);

