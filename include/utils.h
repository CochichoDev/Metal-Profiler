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

size_t itos(int num, char *str);
int64_t parseNum(const char *str);
double parseFloat(char *str);

T_VOID strToUpper(T_PSTR str);

T_UINT numColumnInFile(FILE *file);

T_VOID saveDataMETRICS(const T_PSTR output, G_ARRAY *metrics_array);
T_VOID saveDataRESULTS(const T_PSTR output, G_ARRAY *result_data);
T_ERROR saveDataRESULTBATCH(const T_PSTR output, G_ARRAY *result_array, size_t size_result_array);

size_t strProprietyIdxByPtr(T_PSTR *OPTS, T_PSTR prop);
size_t strProprietyIdxByValue(T_PSTR *OPTS, T_PSTR prop);

CONFIG *const cloneConfig(CONFIG *const cfg);

T_INT uniformRandom(T_INT min, T_INT max);
T_INT binomialRandom(T_UINT n, T_DOUBLE p);
