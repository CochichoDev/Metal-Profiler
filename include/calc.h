#pragma once

#include "api.h"

#include <stddef.h>

#include "global.h"

typedef struct {
    T_VOID *max;
    T_VOID *min;
    T_VOID *median;
} METRICS;

#define CALC_MAX_FROM_RESULTS(T, RESULT_ARRAY, NUM, OUTPUT_ARRAY)      \
    __##T##__calcMaxFromResults(RESULT_ARRAY, NUM, OUTPUT_ARRAY)

#define CALC_METRICS_FROM_ARRAY(T, INPUT_ARRAY, NUM, OUTPUT_METRICS)      \
    __##T##__calcMetricsFromArray(INPUT_ARRAY, NUM, OUTPUT_METRICS)

T_VOID __T_INT__calcMaxFromResults(RESULT *result_array, size_t num, T_INT *max_array);
T_VOID __T_DOUBLE__calcMaxFromResults(RESULT *result_array, size_t num, T_DOUBLE *max_array);

T_VOID __T_INT__calcMetricsFromArray(T_INT *input_array, size_t num, METRICS *output_metrics);
T_VOID __T_DOUBLE__calcMetricsFromArray(T_DOUBLE *input_array, size_t num, METRICS *output_metrics);

void calculateMetrics(void *results);
void calculateDegradation(void *deg, void *iso, void *full);
