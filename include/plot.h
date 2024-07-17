#pragma once

#include "global.h"

#define saveData(T, output, ...) \
    T##_saveData(output, __VA_ARGS__)

void uint64_t_saveData(const char *output, ...);
void double_saveData(const char *output, ...);
void plotBarWErrors(const char *output);
