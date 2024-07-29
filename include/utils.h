#pragma once

#include <stdint.h>
#include <stdio.h>

#include "api/api.h"
#include "calc.h"

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

T_UINT numColumnInFile(FILE *file);

T_VOID saveDataMETRICS(const T_PSTR output, G_ARRAY *metrics_array);
T_VOID saveDataRESULTS(const T_PSTR output, G_ARRAY *result_data);
T_ERROR saveDataRESULTBATCH(const T_PSTR output, G_ARRAY *result_array, size_t size_result_array);
