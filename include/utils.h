#pragma once

#include <stdint.h>

#define isnotblank(c) ((c) > 0x21 && (c) < 0x7E)

int64_t parseNum(char *str);
double parseFloat(char *str);
