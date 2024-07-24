#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "api.h"
#include "results.h"

/*
 *  Simply initializes the struct to all zeros and
 *  the pointer to a memory block big enough to hold
 *  the victim results
 */
void __T_UINT_initializeResults(RESULT *results_ptr, T_UINT num_cycles, const T_PSTR name) {
    if (!results_ptr) {
        perror("Error: Null results pointer cannot be derefenced\n");
        exit(1);
    }
    strcpy(results_ptr->NAME, name);
    results_ptr->DATA = (T_UINT *) malloc(sizeof(T_UINT)*num_cycles);
    results_ptr->NUM = num_cycles;
    results_ptr->TYPE = R_INT;
}
void __T_DOUBLE_initializeResults(RESULT *results_ptr, T_UINT num_cycles, const T_PSTR name) {
    if (!results_ptr) {
        perror("Error: Null results pointer cannot be derefenced\n");
        exit(1);
    }
    strcpy(results_ptr->NAME, name);
    results_ptr->DATA = (T_DOUBLE *) malloc(sizeof(T_DOUBLE)*num_cycles);
    results_ptr->NUM = num_cycles;
    results_ptr->TYPE = R_DOUBLE;
}

void __T_UINT_destroyResults(RESULT *results_ptr) {
    if (!results_ptr) {
        perror("Error: Null results pointer cannot be derefenced\n");
        exit(1);
    }
    free(results_ptr->DATA);
}
void __T_DOUBLE_destroyResults(RESULT *results_ptr) {
    if (!results_ptr) {
        perror("Error: Null results pointer cannot be derefenced\n");
        exit(1);
    }
    free(results_ptr->DATA);
}
