#pragma once

#include "api.h"

/*
void __T_INT_saveDataVAR(const char *output, ...);
void __T_DOUBLE_saveDataVAR(const char *output, ...);
*/

T_VOID plotResults();

void plotScatter(const T_PSTR input, const T_PSTR output);
void plotBarWErrors(const T_PSTR input, const T_PSTR output);
