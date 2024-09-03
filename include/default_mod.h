#ifndef DEFAULT_MOD_H
#define DEFAULT_MOD_H

#include "types.h"
#include "results.h"

void default_BUILD_PROJECT(CONFIG *config);
void default_INIT_BENCH();
void default_RUN_BENCH(RESULT *results);
void default_EXIT_BENCH();

#endif
