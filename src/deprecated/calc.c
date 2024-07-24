#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "calc.h"

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
