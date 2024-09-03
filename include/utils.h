#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>

#include "api/api.h"
#include "calc.h"


/************** MACRO DEFINITION ****************/
#define isnotblank(c) ((c) > 0x21 && (c) < 0x7E)

/*
 * GET_FIRST_CHAR(ptr) : Get the first any character ocurrence it finds, pointed by ptr
 * in case it doesn't find it the pointer will point to the end of the line (input)
 */ 
#define GET_FIRST_CHAR(ptr)        \
    for ( ; *ptr != '\n' ; ptr++)    \
        if (isalnum(*ptr) || *ptr == '#') break

/*
 * GET_FIRST_OCUR(ptr,c ) : Get the first 'c' character ocurrence it finds, pointed by ptr
 * in case it doesn't find it the pointer will point to the end of the line (input)
 */ 
#define GET_FIRST_OCUR(ptr, c)        \
    for ( ; *ptr != c ; ptr++)    \
        if (*ptr == '\n') break

/************** PARSING FUNCTIONS ****************/
size_t itos(int num, char *str);
int64_t parseNum(const char *str);
double parseFloat(char *str);

/************** STRING HANDLING FUNCTIONS ****************/
T_VOID strToUpper(T_PSTR str);
T_PSTR getNameFromPath(T_PSTR path);

/************** FILE HANDLING FUNCTIONS ****************/
T_UINT numColumnInFile(FILE *file);
T_VOID saveDataMETRICS(const T_PSTR output, G_ARRAY *metrics_array);
T_VOID saveDataRESULTS(const T_PSTR output, G_ARRAY *result_array);
T_ERROR saveDataRESULTBATCH(const T_PSTR output, G_ARRAY *result_array, size_t size_result_array);

/************** CONFIG HANDLING FUNCTIONS ****************/
size_t strProprietyIdxByPtr(T_PSTR *OPTS, T_PSTR prop);
size_t strProprietyIdxByValue(T_PSTR *OPTS, T_PSTR prop);
CONFIG *const cloneConfig(CONFIG *const cfg);

/************** MAKEFILE HANDLING FUNCTIONS ****************/
T_ERROR CALL_MAKEFILES(CONFIG *config);
#endif
