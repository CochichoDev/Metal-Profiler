#pragma once

#include "api.h"
void __T_UINT_initializeResults(RESULT *results_ptr, T_UINT num_cycles, const T_PSTR name);
void __T_DOUBLE_initializeResults(RESULT *results_ptr, T_UINT num_cycles, const T_PSTR name);
void __T_UINT_destroyResults(RESULT *results_ptr);
void __T_DOUBLE_destroyResults(RESULT *results_ptr);

void READ_TO_RESULT(T_INT in, RESULT *result, T_CHAR marker);
