/*
 * File: calc.c
 * Calculations and algorithms
 * Author: Diogo Cochicho
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <time.h>

#include "calc.h"
#include "types.h"
#include "arch.h"


/************** RESULT RELATED FUNCTIONS ****************/
T_VOID __T_UINT__calcMaxFromResults(RESULT *result_array, size_t num, T_INT *max_array) {
    for (size_t idx = 0; idx < num; idx++) {
        T_INT max = *((T_INT *)result_array[idx].ARRAY.DATA);

        for (size_t result_idx = 0; result_idx < result_array[idx].ARRAY.SIZE; result_idx++) {
            T_INT cur = *((T_INT *)result_array[idx].ARRAY.DATA + result_idx);
            if (max > cur) max = cur;
        }

        max_array[idx] = max;
    }
}

T_VOID __T_DOUBLE__calcMaxFromResults(RESULT *result_array, size_t num, T_DOUBLE *max_array) {
    for (size_t idx = 0; idx < num; idx++) {
        T_DOUBLE max = *((T_DOUBLE *)result_array[idx].ARRAY.DATA);

        for (size_t result_idx = 0; result_idx < result_array[idx].ARRAY.SIZE; result_idx++) {
            T_DOUBLE cur = *((T_DOUBLE *)result_array[idx].ARRAY.DATA + result_idx);
            if (max > cur) max = cur;
        }

        max_array[idx] = max;
    }
}

/************** STANDARD GENERICS RELATED FUNCTIONS ****************/
/*
 * calcMaxFromArray : Expects an input_array of a G_ARRAY of a std type.
 *                    THe output argument, max_array, should have been already allocated
 *                    and initialized.
 *                      
 */
T_VOID calcMaxFromArray(G_ARRAY *input_array, size_t num_elem, G_ARRAY *max_array) {
    if (num_elem <= 0) return;
    for (size_t idx = 0; idx < num_elem; idx++) {
        switch (input_array[idx].TYPE) {
            case G_INT:
            case G_UINT:
                if (input_array[idx].SIZE == 0) {
                    fprintf(stderr, "Error: It is not possible to calculate the maximum of an array of 0 elements\n");
                    ((T_UINT *)max_array->DATA)[idx] = 0;
                    continue;
                }

                ((T_UINT *)max_array->DATA)[idx] = ((T_UINT *)input_array[idx].DATA)[0];
                T_UINT *max = ((T_UINT *)max_array->DATA+idx);

                for (size_t result_idx = 0; result_idx < input_array[idx].SIZE; result_idx++) {
                    T_UINT cur = *((T_UINT *)input_array[idx].DATA + result_idx);
                    if (*max < cur) *max = cur;
                }
                break;
            case G_DOUBLE:
                if (input_array[idx].SIZE == 0) {
                    fprintf(stderr, "Error: It is not possible to calculate the maximum of an array of 0 elements\n");
                    ((T_DOUBLE *)max_array->DATA)[idx] = 0;
                    continue;
                }

                ((T_DOUBLE *)max_array->DATA)[idx] = ((T_DOUBLE *)input_array[idx].DATA)[0];
                T_DOUBLE *dmax = ((T_DOUBLE *)max_array->DATA+idx);

                for (size_t result_idx = 0; result_idx < input_array[idx].SIZE; result_idx++) {
                    T_DOUBLE cur = *((T_DOUBLE *)input_array[idx].DATA + result_idx);
                    if (*dmax < cur) *dmax = cur;
                }
                break;

            default:
                break;

        }
    }
}

/*
 * calculateDegradation ; It takes an array of G_ARRAY for the data of isolated victim and the same for the
 *                          the full config (it must be of either G_INT, G_UINT or G_DOUBLE), it then returns 
 *                          the calculated degradation data in the deg_array.
 *                          
 *                          The output consists of a G_ARRAY array, in which, each element of the array is a
 *                          copy of an element of result_array where the value is now the degradation.
 *                          
 *                          It is expected for the deg_array to be allocated. 
 */
T_ERROR calculateDegradation(G_ARRAY *garrays_std_iso, size_t size_iso, G_ARRAY *garrays_std_full, size_t size_result, G_ARRAY *garrays_double_deg) {
    G_ARRAY *iso_max = NULL;
    iso_max = malloc(sizeof(G_ARRAY));
    switch (garrays_std_iso->TYPE) {
        case G_INT:
        case G_UINT:
            iso_max->DATA = malloc(sizeof(T_UINT) * size_iso);
            iso_max->SIZE = size_iso;
            iso_max->TYPE = G_UINT;
            break;
        case G_DOUBLE:
            iso_max->DATA = malloc(sizeof(T_DOUBLE) * size_iso);
            iso_max->SIZE = size_iso;
            iso_max->TYPE = G_DOUBLE;
            break;
        default:
            break;
    }

    calcMaxFromArray(garrays_std_iso, size_iso, iso_max);

    
    METRICS iso_metrics;
    initMetricsFromArray(iso_max, "iso_metrics", &iso_metrics);
    if (iso_max != garrays_std_iso) {
        free(iso_max->DATA);
        free(iso_max);
    }

    for (size_t result_idx = 0 ; result_idx < size_result ; result_idx++) {
        for (size_t data_idx = 0; data_idx < garrays_std_full[result_idx].SIZE; data_idx++) {
            switch (garrays_std_full[result_idx].TYPE) {
                case G_INT:
                case G_UINT:
                    if (((T_UINT *) iso_metrics.MEDIAN.DATA)[0] == 0) {
                        ((T_DOUBLE *)garrays_double_deg[result_idx].DATA)[data_idx] = 0;
                        continue;
                    }
                    ((T_DOUBLE *)garrays_double_deg[result_idx].DATA)[data_idx] = \
                        (T_DOUBLE) ((T_UINT *) garrays_std_full[result_idx].DATA)[data_idx] / (T_DOUBLE) ((T_UINT *) iso_metrics.MEDIAN.DATA)[0];
                    break;
                case G_DOUBLE:
                    if (((T_DOUBLE *) iso_metrics.MEDIAN.DATA)[0] == 0) {
                        ((T_DOUBLE *)garrays_double_deg[result_idx].DATA)[data_idx] = 0;
                        continue;
                    }
                    ((T_DOUBLE *)garrays_double_deg[result_idx].DATA)[data_idx] = \
                        ((T_DOUBLE *) garrays_std_full[result_idx].DATA)[data_idx] / ((T_DOUBLE *) iso_metrics.MEDIAN.DATA)[0];
                    break;
                default:
                    break;
            }
        }
    }
    destroyMetrics(&iso_metrics);
    return 0;
}

T_DOUBLE *calculateDegradationNormalized(G_ARRAY *garray_result_iso, G_ARRAY *garray_result_full, T_DOUBLE avg, size_t num) {
    // Calculate vector of degradation
    G_ARRAY *garrays_std_full = calloc(garray_result_full->SIZE, sizeof(G_ARRAY));
    G_ARRAY *garrays_std_deg = calloc(garray_result_full->SIZE, sizeof(G_ARRAY));

    for (size_t result_idx = 0; result_idx < garray_result_full->SIZE; result_idx++) {
        RESULT *result = ((RESULT *) garray_result_full->DATA) + result_idx;
        garrays_std_full[result_idx].DATA = result->ARRAY.DATA;
        garrays_std_full[result_idx].SIZE = result->ARRAY.SIZE;
        garrays_std_full[result_idx].TYPE = result->ARRAY.TYPE;

        garrays_std_deg[result_idx].DATA = calloc(result->ARRAY.SIZE, sizeof(T_DOUBLE));
        garrays_std_deg[result_idx].SIZE = result->ARRAY.SIZE;
        garrays_std_deg[result_idx].TYPE = G_DOUBLE;
    }


    // Since the calculateDegradation takes the result data in an array format
    G_ARRAY *garrays_std_iso = malloc(garray_result_iso->SIZE * sizeof(G_ARRAY));

    // Transform GARRAY of RESULT into GARRAY of std type 
    for (size_t result_idx = 0; result_idx < garray_result_iso->SIZE; result_idx++) {
        garrays_std_iso[result_idx].TYPE = ((RESULT *) (garray_result_iso->DATA))[result_idx].ARRAY.TYPE;
        garrays_std_iso[result_idx].SIZE = ((RESULT *) (garray_result_iso->DATA))[result_idx].ARRAY.SIZE;
        garrays_std_iso[result_idx].DATA = ((RESULT *) (garray_result_iso->DATA))[result_idx].ARRAY.DATA;
    }

    calculateDegradation(garrays_std_iso, garray_result_iso->SIZE, garrays_std_full, garray_result_full->SIZE, garrays_std_deg);

    // Find the maximum of the vector of degradation -> OBJECTIVE
    G_ARRAY garray_std_max_deg = {.SIZE = garray_result_full->SIZE, .TYPE = G_DOUBLE, .DATA = calloc(garray_result_full->SIZE, sizeof(T_DOUBLE))};    
    calcMaxFromArray(garrays_std_deg, garray_result_full->SIZE, &garray_std_max_deg);

    G_ARRAY garray_std_abs_max_deg = {.SIZE = 1, .TYPE = G_DOUBLE, .DATA = malloc(sizeof(T_DOUBLE))};
    calcMaxFromArray(&garray_std_max_deg, 1, &garray_std_abs_max_deg);

    T_DOUBLE deg1 = -((T_DOUBLE *) garray_std_abs_max_deg.DATA)[0];
    avg = avg * ((T_DOUBLE) num / num+1);
    avg += (deg1 / num+1);
    
    deg1 /= avg;
    T_DOUBLE *result = malloc(2 * sizeof(T_DOUBLE));
    result[0] = deg1;
    result[1] = avg;

    for (size_t result_idx = 0; result_idx < garray_result_full->SIZE; result_idx++) 
        free(garrays_std_deg[result_idx].DATA);
    free(garrays_std_iso);
    free(garrays_std_full);
    free(garrays_std_deg);
    free(garray_std_max_deg.DATA);
    free(garray_std_abs_max_deg.DATA);

    return result;
}

/************** METRICS RELATED FUNCTIONS ****************/
/*
 * initMetricsFromArray : Allocates memory for every field of the output_metrics
 *                          and initializes them with the correct metrics calculated
 *                          from the input_array, that should be of a std type.
 *                        The output_metrics struct must already be initialized, it's not
 *                          the responsability of this function.
 */
T_VOID initMetricsFromArray(const G_ARRAY *input_array, const T_PSTR output_name, METRICS *output_metrics) {
    if (!output_metrics) {
        fprintf(stderr, "Error: In order to initialize METRICS please use an already existing struct\n");
        return;
    }
    // Make a local copy of the input_array so that the input doesn't change
    // And initialize output_metrics to the correct size depending on the input type
    T_VOID *local_cpy;
    switch (input_array->TYPE) {
        case G_INT:
        case G_UINT:
            local_cpy = malloc(sizeof(G_UINT)*input_array->SIZE);
            memcpy(local_cpy, input_array->DATA, sizeof(G_UINT) * input_array->SIZE);

            // Process data
            T_UINT *i_data = (T_UINT *) local_cpy;
            for (size_t i = 0 ; i < input_array->SIZE-1 ; i++) {
                for (size_t j = i+1 ; j < input_array->SIZE ; j++) {
                    if (i_data[j] < i_data[i]) {
                        i_data[j] ^= i_data[i];
                        i_data[i] ^= i_data[j];
                        i_data[j] ^= i_data[i];
                    }
                }
            }

            output_metrics->MEDIAN.DATA = malloc(sizeof(T_UINT));
            output_metrics->MEDIAN.SIZE = 1;
            output_metrics->MEDIAN.TYPE = G_UINT;
            output_metrics->MIN.DATA = malloc(sizeof(T_UINT));
            output_metrics->MIN.SIZE = 1;
            output_metrics->MIN.TYPE = G_UINT;
            output_metrics->MAX.DATA = malloc(sizeof(T_UINT));
            output_metrics->MAX.SIZE = 1;
            output_metrics->MAX.TYPE = G_UINT;
            *((T_INT *) output_metrics->MAX.DATA) = i_data[input_array->SIZE-1];
            *((T_INT *) output_metrics->MIN.DATA) = i_data[0];
            *((T_INT *) output_metrics->MEDIAN.DATA) = i_data[input_array->SIZE/2];

            free(local_cpy);
            break;

        case G_DOUBLE:
            local_cpy = malloc(sizeof(T_DOUBLE)*input_array->SIZE);
            memcpy(local_cpy, input_array->DATA, sizeof(T_DOUBLE) * input_array->SIZE);
            //
            // Process data
            T_DOUBLE *f_data = (T_DOUBLE *) local_cpy;
            T_DOUBLE tmp = f_data[0];
            for (size_t i = 0 ; i < input_array->SIZE-1 ; i++) {
                for (size_t j = i+1 ; j < input_array->SIZE ; j++) {
                    if (f_data[j] < f_data[i]) {
                        tmp = f_data[j];
                        f_data[j] = f_data[i];
                        f_data[i] = tmp;
                    }
                }
            }

            output_metrics->MEDIAN.DATA = malloc(sizeof(T_DOUBLE));
            output_metrics->MEDIAN.SIZE = 1;
            output_metrics->MEDIAN.TYPE = G_DOUBLE;
            output_metrics->MIN.DATA = malloc(sizeof(T_DOUBLE));
            output_metrics->MIN.SIZE = 1;
            output_metrics->MIN.TYPE = G_DOUBLE;
            output_metrics->MAX.DATA = malloc(sizeof(T_DOUBLE));
            output_metrics->MAX.SIZE = 1;
            output_metrics->MAX.TYPE = G_DOUBLE;
            *((T_DOUBLE *) output_metrics->MAX.DATA) = f_data[input_array->SIZE-1];
            *((T_DOUBLE *) output_metrics->MIN.DATA) = f_data[0];
            *((T_DOUBLE *) output_metrics->MEDIAN.DATA) = f_data[input_array->SIZE/2];
            
            free(local_cpy);
            break;

        default:
            break;
    }

    strcpy(output_metrics->NAME, output_name);
}

T_VOID destroyMetrics(METRICS *metrics) {
    free(metrics->MAX.DATA);
    metrics->MAX.DATA = NULL;
    metrics->MAX.SIZE = 0;
    free(metrics->MIN.DATA);
    metrics->MIN.DATA = NULL;
    metrics->MIN.SIZE = 0;
    free(metrics->MEDIAN.DATA);
    metrics->MEDIAN.DATA = NULL;
    metrics->MEDIAN.SIZE = 0;
}

/************** CACHE CALC FUNCTIONS ****************/
/*
 * contiguousPages : Calculates the number of contiguos pages that should be
 *                  allocated for each core.
 */
T_UINT contiguousPages(ARCH_DESC *arch) {
    size_t first_b = -1;
    T_UINT bits_size = 0;

    for (size_t cache_idx = 0; cache_idx < arch->CACHE_LVL; ++cache_idx) {
        CACHE_DESC *cache = arch->CACHES + cache_idx;
        

        size_t app_size = __builtin_ctz(cache->SHARED_NUM);
        if (app_size == 0) continue;
        size_t line_off = __builtin_ctz(cache->LINE_SIZE);
        size_t size_off = __builtin_ctz(cache->SIZE);
        size_t way_off = __builtin_ctz(cache->WAYS);
        size_t set_off = size_off - line_off - way_off;


        size_t coloring_b = line_off + set_off - app_size;
        /* In case this is the first shared cache */
        if (first_b == -1 || coloring_b < first_b) {
            first_b = coloring_b;
            if (first_b < __builtin_ctz(arch->PAGE_SIZE)) {
                fprintf(stdout, "Perfect coloring is not possible due to Cache L%ld\n", cache_idx+1);
                first_b = __builtin_ctz(arch->PAGE_SIZE);
            }
        }
        if (bits_size < app_size) bits_size = app_size;

    }
    return 1 << (first_b - __builtin_ctz(arch->PAGE_SIZE));
}

/************** MATH FUNCTIONS ****************/
T_INT uniformRandom(T_INT min, T_INT max) {
    T_INT range = max - min + 1;
    T_UCHAR range_nbit = 0;

    srand(clock());
    while ((1 << range_nbit) < range) {
        range_nbit++;
    }
    // range_nbit has as many bits as needed to represent range
    // since higher order bits in rand() are better distributed
    // I'll take the minimum high-order bits to represent my
    // distribution sampling
    T_UCHAR num_randmax_bits = __builtin_popcount(RAND_MAX);
    T_UCHAR shift = num_randmax_bits - range_nbit;

    T_INT random_num;


    do {
        random_num = rand() >> shift;
    } while (random_num >= range);

    return (random_num + min);
}

T_INT binomialRandom(T_UINT n, T_DOUBLE p) {
    T_INT successes = 0;

    while (n > 0) {
        if (uniformRandom(0, 255) < 255*p) successes++;
        n--;
    }

    return successes;
}
