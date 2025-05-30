#ifndef CALC_H
#define CALC_H

#include "types.h"
#include "results.h"
#include "arch.h"

#include <stddef.h>

/************** TYPE DEFINITIONS ****************/
typedef struct {
    T_STR   NAME;
    G_ARRAY MAX;
    G_ARRAY MIN;
    G_ARRAY MEDIAN;
} METRICS;

/************** MACRO DEFINITIONS ****************/
#define CALC_MAX_FROM_RESULTS(T, RESULT_ARRAY, NUM, OUTPUT_ARRAY)      \
    __##T##__calcMaxFromResults(RESULT_ARRAY, NUM, OUTPUT_ARRAY)

#define CALC_METRICS_FROM_ARRAY(T, GEN_INPUT_ARRAY, NUM, P_NAME, OUTPUT_METRICS)      \
    __##T##__calcMetricsFromArray(INPUT_ARRAY, NUM, P_NAME, OUTPUT_METRICS)


/************** RESULT RELATED FUNCTIONS ****************/
T_VOID __T_UINT__calcMaxFromResults(RESULT *result_array, size_t num, T_INT *max_array);
T_VOID __T_DOUBLE__calcMaxFromResults(RESULT *result_array, size_t num, T_DOUBLE *max_array);

/************** STANDARD GENERICS RELATED FUNCTIONS ****************/
T_VOID calcMaxFromArray(G_ARRAY *input_array, size_t num_elem, G_ARRAY *max_array);
T_ERROR calculateDegradation(G_ARRAY *garrays_std_iso, size_t size_iso, G_ARRAY *garrays_std_full, size_t size_result, G_ARRAY *garrays_double_deg);
T_DOUBLE *calculateDegradationNormalized(G_ARRAY *garray_result_iso, G_ARRAY *garray_result_full, T_DOUBLE avg, size_t num, T_DOUBLE min, T_DOUBLE max);

/************** METRICS RELATED FUNCTIONS ****************/
T_VOID initMetricsFromArray(const G_ARRAY *input_array, const T_PSTR output_name, METRICS *output_metrics);
T_VOID destroyMetrics(METRICS *metrics);

/************** MATH FUNCTIONS ****************/
T_INT uniformRandom(T_INT min, T_INT max);
T_INT binomialRandom(T_UINT n, T_DOUBLE p);




T_UINT contiguousPages(ARCH_DESC *arch);
#endif
