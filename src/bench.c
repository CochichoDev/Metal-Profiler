#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "bench.h"
#include "api.h"
#include "calc.h"
#include "global.h"
#include "utils.h"
#include "cli.h"

T_ERROR runBench(size_t iter, RESULT *result_array) {
    INIT_BENCH();
    for (size_t idx = 0; idx < iter; idx++) {
        cliPrintProgress(idx, iter);
        RESULT *result = RUN_BENCH();
        if (!result) {
            fprintf(stderr, "Error: Couldn't get result data from the module\n");
        }
        size_t result_size = result->ARRAY.SIZE - IGNORE_LIMIT;
        switch (result->ARRAY.TYPE) {
            case G_INT:
                INITIALIZE_RESULTS(T_UINT, result_array+idx, result_size, result->NAME);
                memcpy(result_array[idx].ARRAY.DATA, result->ARRAY.DATA + IGNORE_LIMIT*sizeof(T_UINT), sizeof(T_UINT)*result_size);
                break;
            case G_UINT:
                INITIALIZE_RESULTS(T_UINT, result_array+idx, result_size, result->NAME);
                memcpy(result_array[idx].ARRAY.DATA, result->ARRAY.DATA + IGNORE_LIMIT*sizeof(T_UINT), sizeof(T_UINT)*result_size);
                break;
            case G_DOUBLE:
                INITIALIZE_RESULTS(T_DOUBLE, result_array+idx, result_size, result->NAME);
                memcpy(result_array[idx].ARRAY.DATA, result->ARRAY.DATA + IGNORE_LIMIT*sizeof(T_DOUBLE), sizeof(T_DOUBLE)*result_size);
            default:
                break;
        }
    }
    EXIT_BENCH();
    return 0;
}

/*
 * processResults : It performs the necessary data treatment that are specified
 *                  in the global OUTPUT_LIST and saves the data in files whose names
 *                  follow the convention.
 *
 *                  The input result_array must be well initialized, it is a generic array
 *                  of RESULT elements.
 */
T_VOID processResults(G_ARRAY *result_array) {
    if (result_array->SIZE <= 0) {
        fprintf(stderr, "Error: The number of results must be positive\n");
        return;
    }

    T_FLAG raw_saved = FALSE;
    T_FLAG degradation_saved = FALSE;

    G_ARRAY raw_metrics_array = {.SIZE = result_array->SIZE, .TYPE = G_METRICS, .DATA = NULL};
    G_ARRAY deg_metrics_array = {.SIZE = result_array->SIZE, .TYPE = G_METRICS, .DATA = NULL};

    T_STR data_file_name_buf = { 0 };
    T_STR metric_file_name_buf = { 0 };

    OUTPUT_LIST *iter = OUTPUT_LIST_SELECTED;
    while (iter != NULL) {
        strcpy(data_file_name_buf, iter->OUT->NAME);

        T_STR metric_name;
        switch (iter->OUT->DATA_TYPE) {
            case RAW:
                if (raw_saved) break;        // In case the raw data has been processed no need to do it again

                if (!raw_metrics_array.DATA)
                    raw_metrics_array.DATA = malloc(sizeof(METRICS) * raw_metrics_array.SIZE);
                
                METRICS *raw_metrics = raw_metrics_array.DATA;          // Just for easier comprehension

                strcat(data_file_name_buf, "_raw");
                strcpy(metric_file_name_buf, data_file_name_buf);
                strcat(metric_file_name_buf, "_metrics");
                
                saveDataRESULTS(data_file_name_buf, result_array);
                
                for (size_t idx = 0; idx < result_array->SIZE; idx++) {
                    sprintf(metric_name, "METRIC_%ld", idx);
                    RESULT *cur_result = (RESULT *) result_array->DATA + idx;
                    initMetricsFromArray(&cur_result->ARRAY, metric_name, raw_metrics+idx);
                }

                saveDataMETRICS(metric_file_name_buf, &raw_metrics_array);
                for (size_t idx = 0; idx < result_array->SIZE; idx++) {
                    destroyMetrics(raw_metrics+idx);
                }
                free(raw_metrics);
                           
                    
                raw_saved = TRUE;
                break;

            case DEGRADATION:
                if (degradation_saved) break;

                // Calculate MEDIAN of isolation run
                G_ARRAY iso_result_array = {.DATA = malloc(sizeof(RESULT) * result_array->SIZE), .SIZE = result_array->SIZE, .TYPE = G_RESULT};

                // Change configuration to only compile the isolated victim
                CONFIG *cfg_iso = calloc(1, sizeof(CONFIG));
                cfg_iso->NUM = 1;
                cfg_iso->VICTIM_ID = INPUT_CONFIG->VICTIM_ID;
                GET_COMP_BY_IDX(INPUT_CONFIG, cfg_iso->VICTIM_ID, cfg_iso->COMPS);
                cfg_iso->VICTIM_ID = 0;         // Since it is the only component it possesses
                BUILD_PROJECT(cfg_iso);

                runBench(iso_result_array.SIZE, iso_result_array.DATA);
                free(cfg_iso);

                G_ARRAY *deg_array = calloc(result_array->SIZE, sizeof(G_ARRAY));
                // Since the calculateDegradation takes the result data in an array format
                G_ARRAY *input_result_array = malloc(result_array->SIZE * sizeof(G_ARRAY));
                G_ARRAY *input_iso_array = malloc(iso_result_array.SIZE * sizeof(G_ARRAY));

                // Transform RESULT array into std type array
                for (size_t result_idx = 0; result_idx < result_array->SIZE; result_idx++) {
                    input_result_array[result_idx].TYPE = ((RESULT *)result_array->DATA)[result_idx].ARRAY.TYPE;
                    input_result_array[result_idx].SIZE = ((RESULT *)result_array->DATA)[result_idx].ARRAY.SIZE;
                    input_result_array[result_idx].DATA = ((RESULT *)result_array->DATA)[result_idx].ARRAY.DATA;

                    input_iso_array[result_idx].TYPE = ((RESULT *) (iso_result_array.DATA))[result_idx].ARRAY.TYPE;
                    input_iso_array[result_idx].SIZE = ((RESULT *) (iso_result_array.DATA))[result_idx].ARRAY.SIZE;
                    input_iso_array[result_idx].DATA = ((RESULT *) (iso_result_array.DATA))[result_idx].ARRAY.DATA;
                    
                    deg_array[result_idx].DATA = malloc(((RESULT *)result_array->DATA)[result_idx].ARRAY.SIZE * sizeof(T_DOUBLE));
                    deg_array[result_idx].TYPE = G_DOUBLE;
                    deg_array[result_idx].SIZE = ((RESULT *)result_array->DATA)[result_idx].ARRAY.SIZE;
                }

                calculateDegradation(input_iso_array, result_array->SIZE, input_result_array, result_array->SIZE, deg_array);

                free(input_iso_array);
                free(input_result_array);


                // Need to case G_ARRAY of G_DOUBLE to G_ARRAY of G_RESULT)
                // Instead of allocating more memory, simply use the one for the iso_result_array (same type and size
                // Deallocate the previousa allocated memory for each DATA of RESULT in iso_result_array
                for (size_t result_idx = 0; result_idx < result_array->SIZE; result_idx++) {
                    DESTROY_RESULTS(T_UINT, ((RESULT *)iso_result_array.DATA) + result_idx);          // Doesn't matter what type
                }
                G_ARRAY *deg_result_array = &iso_result_array;
                for (size_t deg_result_idx = 0 ; deg_result_idx < deg_result_array->SIZE ; deg_result_idx++) {
                    RESULT *result_data = ((RESULT *)deg_result_array->DATA) + deg_result_idx;
                    result_data->ARRAY.TYPE = G_DOUBLE;
                    result_data->ARRAY.SIZE = deg_array[deg_result_idx].SIZE;
                    result_data->ARRAY.DATA = deg_array[deg_result_idx].DATA;
                    strcpy(result_data->NAME, (((RESULT *)result_array->DATA)[deg_result_idx].NAME));
                    strcat(result_data->NAME, "_DEG");
                }
                
                strcat(data_file_name_buf, "_deg");
                saveDataRESULTS(data_file_name_buf, deg_result_array);

                if (!deg_metrics_array.DATA) {
                    deg_metrics_array.DATA = malloc(sizeof(METRICS) * deg_metrics_array.SIZE);
                }

                METRICS *deg_metrics = deg_metrics_array.DATA;
                
                
                for (size_t idx = 0; idx < result_array->SIZE; idx++) {
                    sprintf(metric_name, "METRIC_%ld", idx);
                    initMetricsFromArray(deg_array + idx, metric_name, deg_metrics+idx);
                }

                strcpy(metric_file_name_buf, data_file_name_buf);
                strcat(metric_file_name_buf, "_metrics");
                saveDataMETRICS(metric_file_name_buf, &deg_metrics_array);
                for (size_t idx = 0; idx < result_array->SIZE; idx++) {
                    destroyMetrics(deg_metrics+idx);
                }

                // Clear memory allocations
                for (size_t result_idx = 0; result_idx < result_array->SIZE; result_idx++) {
                    free(deg_array[result_idx].DATA);
                }
                free(iso_result_array.DATA);
                free(deg_metrics_array.DATA);
                free(deg_array);

                degradation_saved = TRUE;

                break;
        }

        iter = iter->NEXT;
    }
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
