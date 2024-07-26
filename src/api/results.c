#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include "api.h"
#include "api/api.h"
#include "results.h"

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
    results_ptr->DATA = malloc(sizeof(T_UINT)*num_cycles);
    results_ptr->NUM = num_cycles;
    results_ptr->TYPE = R_UINT;
}
void __T_DOUBLE_initializeResults(RESULT *results_ptr, T_UINT num_cycles, const T_PSTR name) {
    results_ptr = malloc(sizeof(RESULT));
    if (!results_ptr) {
        fprintf(stderr, "Error: Null results pointer cannot be derefenced\n");
        exit(1);
    }
    strcpy(results_ptr->NAME, name);
    results_ptr->DATA = malloc(sizeof(T_DOUBLE)*num_cycles);
    results_ptr->NUM = num_cycles;
    results_ptr->TYPE = R_DOUBLE;
}

void __T_UINT_destroyResults(RESULT *results_ptr) {
    if (!results_ptr) {
        fprintf(stderr, "Error: Null results pointer cannot be derefenced\n");
        exit(1);
    }
    free(results_ptr->DATA);
}
void __T_DOUBLE_destroyResults(RESULT *results_ptr) {
    if (!results_ptr) {
        fprintf(stderr, "Error: Null results pointer cannot be derefenced\n");
        exit(1);
    }
    free(results_ptr->DATA);
}

void READ_TO_RESULT(T_INT in, RESULT *result, T_CHAR marker) {
    T_CHAR buf[256];
    
    volatile FLAG stop = 0;
    T_UINT read_bytes = 0;
    for (uint32_t idx = 0 ; stop == 0 ; ) {
        read_bytes = read(in,buf,255); 
        buf[read_bytes]='\0';          
        if (buf[0] == marker) stop=1;
        if (isdigit(buf[0])) {
            switch (result->TYPE) {
                case R_INT:
                    sscanf(buf, "%u", ((T_UINT *)result->DATA)+idx);
                    //printf("%u\n", *(((T_UINT *)result->DATA)+idx));
                    break;
                case R_UINT:
                    sscanf(buf, "%u", ((T_UINT *)result->DATA)+idx);
                    break;
                case R_DOUBLE:
                    sscanf(buf, "%lf", (T_DOUBLE *)(result->DATA+idx));
            }
            idx++;
        }
    }
}
