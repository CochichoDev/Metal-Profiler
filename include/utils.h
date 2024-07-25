#pragma once

#include <stdint.h>

#include "api/api.h"

#define isnotblank(c) ((c) > 0x21 && (c) < 0x7E)


/*
 * Function declaration
 */
T_PSTR getNameFromPath(T_PSTR path);

void loadAvailableArchs();
void loadAvailableConfigs();

int64_t parseNum(char *str);
double parseFloat(char *str);

T_VOID strToUpper(T_PSTR str);
