/*
 * File: result.c
 * RESULT type management
 * Author: Diogo Cochicho
 */

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>

#include "api.h"
#include "results.h"
#include "global.h"
#include "generics.h"

/************** RESULT INITIALIZATION ****************/
/*
 *  Simply initializes the struct to all zeros and
 *  the pointer to a memory block big enough to hold
 *  the victim results
 */
void __T_UINT_initializeResults(RESULT *results_ptr, T_UINT num_cycles, const T_PSTR name) {
    if (!results_ptr) {
        fprintf(stderr, "Error: Null results pointer cannot be derefenced\n");
        exit(1);
    }
    strcpy(results_ptr->NAME, name);
    INIT_GENERIC(T_UINT, &(results_ptr->ARRAY), num_cycles);
}
void __T_DOUBLE_initializeResults(RESULT *results_ptr, T_UINT num_cycles, const T_PSTR name) {
    if (!results_ptr) {
        fprintf(stderr, "Error: Null results pointer cannot be derefenced\n");
        exit(1);
    }
    strcpy(results_ptr->NAME, name);
    INIT_GENERIC(T_DOUBLE, &(results_ptr->ARRAY), num_cycles);
}

/************** RESULT DESTRUCTION ****************/
T_VOID DESTROY_RESULTS(RESULT *result_ptr) {
    if (!result_ptr) {
        fprintf(stderr, "Error: Null results pointer cannot be derefenced\n");
        exit(1);
    }
    DESTROY_GENERIC(&(result_ptr->ARRAY));
}


