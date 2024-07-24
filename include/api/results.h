#pragma once

#include "api.h"
void __uint32_t_initializeResults(RESULT *results_ptr, T_UINT num_cycles, const T_PSTR name);
void __double_initializeResults(RESULT *results_ptr, T_UINT num_cycles, const T_PSTR name);
void __uint64_t_destroyResults(RESULT *results_ptr);
void __double_destroyResults(RESULT *results_ptr);
