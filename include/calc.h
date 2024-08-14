#pragma once

#include "api.h"

#include <stddef.h>

typedef struct {
    T_STR   NAME;
    G_ARRAY MAX;
    G_ARRAY MIN;
    G_ARRAY MEDIAN;
} METRICS;

#define CALC_MAX_FROM_RESULTS(T, RESULT_ARRAY, NUM, OUTPUT_ARRAY)      \
    __##T##__calcMaxFromResults(RESULT_ARRAY, NUM, OUTPUT_ARRAY)

#define CALC_METRICS_FROM_ARRAY(T, GEN_INPUT_ARRAY, NUM, P_NAME, OUTPUT_METRICS)      \
    __##T##__calcMetricsFromArray(INPUT_ARRAY, NUM, P_NAME, OUTPUT_METRICS)

T_VOID __T_UINT__calcMaxFromResults(RESULT *result_array, size_t num, T_INT *max_array);
T_VOID __T_DOUBLE__calcMaxFromResults(RESULT *result_array, size_t num, T_DOUBLE *max_array);

T_VOID calcMaxFromArray(G_ARRAY *input_array, size_t num_elem, G_ARRAY *max_array);

T_VOID initMetricsFromArray(const G_ARRAY *input_array, const T_PSTR output_name, METRICS *output_metrics);
T_VOID destroyMetrics(METRICS *metrics);

T_ERROR calculateDegradation(G_ARRAY *iso_data, size_t size_iso, G_ARRAY *result_array, size_t size_result, G_ARRAY *deg_array);
