#pragma once

#include "global.h"

#define SAVE_DATA_RESULTS(T, output, p_RESULT, size) \
    __##T##_saveDataRESULTS(output, p_RESULT, size)

#define SAVE_DATA_VAR(T, output, ...) \
    __##T##_saveDataVAR(output, __VA_ARGS__)

void __T_INT_saveDataRESULTS(const char *output, RESULT *data, size_t size);
void __T_DOUBLE_saveDataRESULTS(const char *output, RESULT *data, size_t size);

void __T_INT_saveDataVAR(const char *output, ...);
void __T_DOUBLE_saveDataVAR(const char *output, ...);
void plotBarWErrors(const char *output);
