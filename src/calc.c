#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

#include "calc.h"
#include "api.h"
#include "apistate.h"

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
*/

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
    G_ARRAY iso_max;
    switch (garrays_std_iso->TYPE) {
        case G_INT:
        case G_UINT:
            iso_max.DATA = malloc(sizeof(T_UINT) * size_iso);
            iso_max.SIZE = size_iso;
            iso_max.TYPE = G_UINT;
            break;
        case G_DOUBLE:
            iso_max.DATA = malloc(sizeof(T_DOUBLE) * size_iso);
            iso_max.SIZE = size_iso;
            iso_max.TYPE = G_DOUBLE;
            break;
        default:
            break;
    }

    calcMaxFromArray(garrays_std_iso, size_iso, &iso_max);
    
    METRICS iso_metrics;
    initMetricsFromArray(&iso_max, "iso_metrics", &iso_metrics);
    free(iso_max.DATA);

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

