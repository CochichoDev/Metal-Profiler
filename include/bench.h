#pragma once

#include "global.h"
#include "tty.h"

void uint64_t_initializeResults(void *results_ptr, uint64_t num_cycles, const char *name);
void double_initializeResults(void *results_ptr, uint64_t num_cycles, const char *name);
void uint64_t_destroyResults(void *results_ptr);
void double_destroyResults(void *results_ptr);

uint64_t benchFullConfig(CoreConfig **config, ttyFD tty);
uint64_t benchIsolationConfig(CoreConfig **config, ttyFD tty);
