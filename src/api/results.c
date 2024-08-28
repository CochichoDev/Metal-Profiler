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

/************** RESULT MANIPULATION ****************/
void READ_TO_RESULT(T_INT in, T_CHAR marker, RESULT *results) {
    assert(OUTPUT_LIST_SELECTED != NULL);
    assert(results != NULL);

    T_CHAR buf[256];

    size_t numResults = 0;
    for (OUTPUT_LIST *out_ptr = OUTPUT_LIST_SELECTED; out_ptr != NULL; out_ptr = out_ptr->NEXT, ++numResults);
    

    volatile T_FLAG stop = 0;
    T_UINT read_bytes = 0;
    for (uint32_t idx = 0, total = 0 ; stop == 0 ; ) {
        read_bytes = read(in,buf,255); 
        buf[read_bytes]='\0';          
        puts(buf);
        if (buf[0] == marker) {
            stop=1;
        }
        if (isdigit(buf[0])) {
            if (idx >= results[total % numResults].ARRAY.SIZE)
                goto CONTINUE;
            switch (results[total % numResults].ARRAY.TYPE) {
                case G_INT:
                    sscanf(buf, "%u", ((T_UINT *)results[total % numResults].ARRAY.DATA)+idx);
                    //printf("%u\n", *(((T_UINT *)result->ARRAY.DATA)+idx));
                    break;
                case G_UINT:
                    sscanf(buf, "%u", ((T_UINT *)results[total % numResults].ARRAY.DATA)+idx);
                    //printf("%u\n", *(((T_UINT *)result->ARRAY.DATA)+idx));
                    break;
                case G_DOUBLE:
                    sscanf(buf, "%lf", ((T_DOUBLE *)results[total % numResults].ARRAY.DATA)+idx);
                default:
                    break;
            } 
        CONTINUE:
            total++;
            idx = total/numResults;
        }
    }
}

