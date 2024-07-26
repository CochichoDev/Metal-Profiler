#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "bench.h"
#include "api.h"
#include "calc.h"
#include "global.h"
#include "plot.h"
#include "results.h"

T_ERROR runBench(TERM *term, size_t iter, RESULT *result_array) {
    INIT_BENCH();
    for (size_t idx = 0; idx < iter; idx++) {
        //cliPrintProgress(term, idx, iter);
        RESULT *result = RUN_BENCH();
        if (!result) {
            fprintf(stderr, "Error: Couldn't get result data from the module\n");
        }
        switch (result->TYPE) {
            case R_INT:
                INITIALIZE_RESULTS(T_UINT, result_array+idx, result->NUM, result->NAME);
                memcpy(result_array[idx].DATA, result->DATA, sizeof(T_UINT)*result->NUM);
                break;
            case R_UINT:
                INITIALIZE_RESULTS(T_UINT, result_array+idx, result->NUM, result->NAME);
                memcpy(result_array[idx].DATA, result->DATA, sizeof(T_UINT)*result->NUM);
                break;
            case R_DOUBLE:
                INITIALIZE_RESULTS(T_DOUBLE, result_array+idx, result->NUM, result->NAME);
                memcpy(result_array[idx].DATA, result->DATA, sizeof(T_DOUBLE)*result->NUM);
        }
    }
    EXIT_BENCH();
    return 0;
}

T_VOID processResults(RESULT *result_array, size_t num) {
    if (num <= 0) {
        fprintf(stderr, "Error: The number of results must be positive\n");
        return;
    }

    T_VOID *max_values;
    METRICS result_metrics;

    /*
    switch (result_array[0].TYPE) {
        case R_INT:
            max_values = malloc(sizeof(T_INT)*num);
            CALC_MAX_FROM_RESULTS(T_INT, result_array, num, max_values);

            result_metrics.max = (T_INT *) malloc(sizeof(T_INT));
            result_metrics.min = (T_INT *) malloc(sizeof(T_INT));
            result_metrics.median = (T_INT *) malloc(sizeof(T_INT));

            CALC_METRICS_FROM_ARRAY(T_INT, max_values, num, &result_metrics);
            break;
        case R_DOUBLE:
            max_values = malloc(sizeof(T_DOUBLE)*num);
            CALC_MAX_FROM_RESULTS(T_DOUBLE, result_array, num, max_values);

            result_metrics.max = (T_DOUBLE *) malloc(sizeof(T_DOUBLE));
            result_metrics.min = (T_DOUBLE *) malloc(sizeof(T_DOUBLE));
            result_metrics.median = (T_DOUBLE *) malloc(sizeof(T_DOUBLE));

            CALC_METRICS_FROM_ARRAY(T_DOUBLE, max_values, num, &result_metrics);
    }
    */


    OUTPUT_LIST *iter = &OUTPUT_LIST_SELECTED;
    while (iter != NULL) {
        switch (iter->OUT->DATA_TYPE) {
            case CYCLES:
                SAVE_DATA_RESULTS(T_INT, iter->OUT->NAME, result_array, num);
                break;
            case DEGRADATION:
                break;
        }

        switch (iter->OUT->GRAPH_TYPE) {
            case SCATTER:
                break;
            case BARWERROR:
                break;
        }
        iter = iter->NEXT;
    }

    /*
    free(max_values);
    free(result_metrics.max);
    free(result_metrics.min);
    free(result_metrics.median);
    */
}

/*
uint64_t benchFullConfig(CoreConfig **config, ttyFD tty) {
    char buf[256];
    char script_query[256] = T32SCRIPT;
    uint16_t read_bytes;

    for (size_t idx = 0 ; idx < NUM_CORES ; idx++) {
        if (config)
            strcat(script_query, " \"TRUE\"");
        else
            strcat(script_query, " \"FALSE\"");
    }

    executeTrace32Script(script_query);

    volatile uint8_t STOP=FALSE;
    uint64_t max = 0, curr = 0;
    for (uint32_t idx = 0 ; STOP == FALSE ; idx++) {
        read_bytes = read(tty.fd,buf,255); 
        buf[read_bytes]=0;          
        if (buf[0]=='F') STOP=TRUE;
        if (isdigit(buf[0])) {
            if (idx < IGNORE_LIMIT) continue;       // This is important to ensure we are working in the nominal scenario
            sscanf(buf, "%lu", &curr);
            if (curr > max) max = curr;
        }
    }

    printf("%lu\n", max);
    return max;
}

uint64_t benchIsolationConfig(CoreConfig **config, ttyFD tty) {
    char buf[256];
    char script_query[256] = T32SCRIPT;
    strcat(script_query, " \"TRUE\"");
    strcat(script_query, " \"FALSE\"");
    strcat(script_query, " \"FALSE\"");
    strcat(script_query, " \"FALSE\"");
    uint16_t read_bytes;

    executeTrace32Script(script_query);

    volatile uint8_t STOP=FALSE;
    uint64_t max = 0, curr = 0;
    for (uint32_t idx = 0 ; STOP == FALSE ; idx++) {
        read_bytes = read(tty.fd,buf,255); 
        buf[read_bytes]=0;          
        if (buf[0]=='F') STOP=TRUE;
        if (isdigit(buf[0])) {
            if (idx < IGNORE_LIMIT) continue;       // This is important to ensure we are working in the nominal scenario
            sscanf(buf, "%lu", &curr);

            if (curr > max) max = curr;
        }
    }

    printf("%lu\n", max);
    return max;
}
*/
