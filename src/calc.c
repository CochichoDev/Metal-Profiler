#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "calc.h"
#include "api.h"

T_VOID __T_INT__calcMaxFromResults(RESULT *result_array, size_t num, T_INT *max_array) {
    for (size_t idx = 0; idx < num; idx++) {
        T_INT max = *((T_INT *)result_array[idx].DATA);

        for (size_t result_idx = 0; result_idx < result_array[idx].NUM; result_idx++) {
            T_INT cur = *((T_INT *)result_array[idx].DATA + result_idx);
            if (max > cur) max = cur;
        }

        max_array[idx] = max;
    }
}

T_VOID __T_DOUBLE__calcMaxFromResults(RESULT *result_array, size_t num, T_DOUBLE *max_array) {
    for (size_t idx = 0; idx < num; idx++) {
        T_DOUBLE max = *((T_DOUBLE *)result_array[idx].DATA);

        for (size_t result_idx = 0; result_idx < result_array[idx].NUM; result_idx++) {
            T_DOUBLE cur = *((T_DOUBLE *)result_array[idx].DATA + result_idx);
            if (max > cur) max = cur;
        }

        max_array[idx] = max;
    }
}

T_VOID __T_INT__calcMetricsFromArray(T_INT *input_array, size_t num, METRICS *output_metrics) {
    T_INT input_cpy[num];
    memcpy(input_cpy, input_array, num*sizeof(T_INT));

    for (size_t i = 0 ; i < num-1 ; i++) {
        for (size_t j = i+1 ; j < num ; j++) {
            if (input_cpy[j] < input_cpy[i]) {
                input_cpy[j] ^= input_cpy[i];
                input_cpy[i] ^= input_cpy[j];
                input_cpy[j] ^= input_cpy[i];
            }
        }
    }

    *((T_INT *) output_metrics->max) = input_cpy[num-1];
    *((T_INT *) output_metrics->min) = input_cpy[0];
    *((T_INT *) output_metrics->median) = input_cpy[num/2];
}

T_VOID __T_DOUBLE__calcMetricsFromArray(T_DOUBLE *input_array, size_t num, METRICS *output_metrics) {
    T_DOUBLE input_cpy[num];
    memcpy(input_cpy, input_array, num*sizeof(T_DOUBLE));

    T_DOUBLE tmp;
    for (size_t i = 0 ; i < num-1 ; i++) {
        for (size_t j = i+1 ; j < num ; j++) {
            if (input_cpy[j] < input_cpy[i]) {
                tmp = input_cpy[j];
                input_cpy[j] = input_cpy[i];
                input_cpy[i] = tmp;
            }
        }
    }

    *((T_DOUBLE *) output_metrics->max) = input_cpy[num-1];
    *((T_DOUBLE *) output_metrics->min) = input_cpy[0];
    *((T_DOUBLE *) output_metrics->median) = input_cpy[num/2];
}

/*
void calculateMetrics(void *results_ptr) {
    RESULTS(uint64_t) *results = results_ptr;
    // Sort result's cycles in increasing value
    for (size_t i = 0 ; i < results->num_cycles -1 ; i++) {
        for (size_t j = i+1 ; j < results->num_cycles ; j++) {
            if (results->cycles[j] < results->cycles[i]) {
                results->cycles[j] ^= results->cycles[i];
                results->cycles[i] ^= results->cycles[j];
                results->cycles[j] ^= results->cycles[i];
            }
        }
    }

    results->median = results->cycles[results->num_cycles/2];
    results->max = results->cycles[results->num_cycles-1];
    results->min = results->cycles[0];
}

void calculateDegradation(void *deg_ptr, void *iso_ptr, void *full_ptr) {
    RESULTS(double) *deg = deg_ptr;
    RESULTS(uint64_t) *iso = iso_ptr, *full = full_ptr;

    if (!iso->median) {
        perror("Error: No reference available\n");
        exit(1);
    }
    if (!full->cycles) {
        perror("Error: No results provided\n");
        exit(1);
    }
    if (full->num_cycles != deg->num_cycles) {
        perror("Error: Number of interference results and degradation results does not match\n");
        exit(1);
    }


    for (size_t idx = 0 ; idx < deg->num_cycles ; idx++) {
        deg->cycles[idx] = ((double) full->cycles[idx]) / ((double) iso->median);
    }

    
    // Sort result's cycles in increasing value
    double tmp = 0;
    for (size_t i = 0 ; i < deg->num_cycles -1 ; i++) {
        for (size_t j = i+1 ; j < deg->num_cycles ; j++) {
            if (deg->cycles[j] < deg->cycles[i]) {
                tmp = deg->cycles[j];
                deg->cycles[j] = deg->cycles[i];
                deg->cycles[i] = tmp;
            }
        }
    }

    deg->median = deg->cycles[deg->num_cycles/2];
    deg->max = deg->cycles[deg->num_cycles-1];
    deg->min = deg->cycles[0];
}
*/
