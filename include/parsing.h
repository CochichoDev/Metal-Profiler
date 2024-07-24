/*
 * Developed by Diogo Cochicho
 */

#pragma once

#include "global.h"
#include <stdio.h>

/*
 * Macro definition
 */
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

/*
 * Function declarations
 */
//CoreConfig *parseCoreConfig(FILE *fd);
CONFIG *parseConfig(FILE *fd);
