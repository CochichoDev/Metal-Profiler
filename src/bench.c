#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "bench.h"
#include "api.h"
#include "apistate.h"
#include "calc.h"
#include "global.h"
#include "utils.h"
#include "state.h"

#define IGNORE_NUM 10

T_ERROR runBench(size_t iter, RESULT *results_input) {
    INIT_BENCH();
    for (size_t idx = 0; idx < iter; idx++) {
        //cliPrintProgress(idx, iter);
        RESULT *result = RUN_BENCH();
        if (!result) {
            fprintf(stderr, "Error: Couldn't get result data from the module\n");
        }
        switch (result->ARRAY.TYPE) {
            case G_INT:
                INITIALIZE_RESULTS(T_UINT, results_input+idx, result->ARRAY.SIZE - IGNORE_LIMIT, result->NAME);
                memcpy(results_input[idx].ARRAY.DATA, result->ARRAY.DATA + IGNORE_LIMIT*sizeof(T_UINT), sizeof(T_UINT)*(result->ARRAY.SIZE - IGNORE_LIMIT));
                break;
            case G_UINT:
                INITIALIZE_RESULTS(T_UINT, results_input+idx, result->ARRAY.SIZE - IGNORE_LIMIT, result->NAME);
                memcpy(results_input[idx].ARRAY.DATA, result->ARRAY.DATA + IGNORE_LIMIT*sizeof(T_UINT), sizeof(T_UINT)*(result->ARRAY.SIZE - IGNORE_LIMIT));
    


                break;
            case G_DOUBLE:
                INITIALIZE_RESULTS(T_DOUBLE, results_input+idx, result->ARRAY.SIZE - IGNORE_LIMIT, result->NAME);
                memcpy(results_input[idx].ARRAY.DATA, result->ARRAY.DATA + IGNORE_LIMIT*sizeof(T_DOUBLE), sizeof(T_DOUBLE)*(result->ARRAY.SIZE - IGNORE_LIMIT));
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
 *                  The input garray_result_input must be well initialized, it is a generic array
 *                  of RESULT elements.
 */
T_VOID processResults(G_ARRAY *garray_result_input) {
    if (garray_result_input->SIZE <= 0) {
        fprintf(stderr, "Error: The number of results must be positive\n");
        return;
    }

    T_FLAG raw_saved = FALSE;
    T_FLAG degradation_saved = FALSE;

    G_ARRAY raw_metrics_array = {.SIZE = garray_result_input->SIZE, .TYPE = G_METRICS, .DATA = NULL};
    G_ARRAY deg_metrics_array = {.SIZE = garray_result_input->SIZE, .TYPE = G_METRICS, .DATA = NULL};

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
                
                saveDataRESULTS(data_file_name_buf, garray_result_input);
                
                for (size_t idx = 0; idx < garray_result_input->SIZE; idx++) {
                    sprintf(metric_name, "METRIC_%ld", idx);
                    RESULT *cur_result = (RESULT *) garray_result_input->DATA + idx;
                    initMetricsFromArray(&cur_result->ARRAY, metric_name, raw_metrics+idx);
                }

                saveDataMETRICS(metric_file_name_buf, &raw_metrics_array);
                for (size_t idx = 0; idx < garray_result_input->SIZE; idx++) {
                    destroyMetrics(raw_metrics+idx);
                }
                free(raw_metrics);
                           
                    
                raw_saved = TRUE;
                break;

            case DEGRADATION:
                if (degradation_saved) break;

                G_ARRAY *garrays_std_deg = calloc(garray_result_input->SIZE, sizeof(G_ARRAY));
                G_ARRAY *garrays_std_input = calloc(garray_result_input->SIZE, sizeof(G_ARRAY));

                // Initialize garrays_std_deg
                for (size_t result_idx = 0; result_idx < garray_result_input->SIZE; result_idx++) {
                    garrays_std_input[result_idx].DATA = ((RESULT *) garray_result_input->DATA)[result_idx].ARRAY.DATA;
                    garrays_std_input[result_idx].SIZE = ((RESULT *) garray_result_input->DATA)[result_idx].ARRAY.SIZE;
                    garrays_std_input[result_idx].TYPE = ((RESULT *) garray_result_input->DATA)[result_idx].ARRAY.TYPE;

                    garrays_std_deg[result_idx].DATA = malloc(((RESULT *)garray_result_input->DATA)[result_idx].ARRAY.SIZE * sizeof(T_DOUBLE));
                    garrays_std_deg[result_idx].TYPE = G_DOUBLE;
                    garrays_std_deg[result_idx].SIZE = ((RESULT *)garray_result_input->DATA)[result_idx].ARRAY.SIZE;
                }

                computeInterferenceDegradation(garrays_std_input, garray_result_input->SIZE, garrays_std_deg);
                free(garrays_std_input);

                G_ARRAY garray_result_deg = {.DATA = calloc(garray_result_input->SIZE, sizeof(RESULT)), .SIZE = garray_result_input->SIZE, .TYPE = G_RESULT };
                for (size_t deg_result_idx = 0 ; deg_result_idx < garray_result_deg.SIZE ; deg_result_idx++) {
                    RESULT *result_data = ((RESULT *) garray_result_deg.DATA) + deg_result_idx;
                    result_data->ARRAY.TYPE = G_DOUBLE;
                    result_data->ARRAY.SIZE = garrays_std_deg[deg_result_idx].SIZE;
                    result_data->ARRAY.DATA = garrays_std_deg[deg_result_idx].DATA;
                    strcpy(result_data->NAME, (((RESULT *) garray_result_input->DATA)[deg_result_idx].NAME));
                    strcat(result_data->NAME, "_DEG");
                }
                
                strcat(data_file_name_buf, "_deg");
                saveDataRESULTS(data_file_name_buf, &garray_result_deg);
                free(garray_result_deg.DATA);


                if (!deg_metrics_array.DATA) {
                    deg_metrics_array.DATA = malloc(sizeof(METRICS) * deg_metrics_array.SIZE);
                }

                METRICS *deg_metrics = deg_metrics_array.DATA;
                
                
                for (size_t idx = 0; idx < garray_result_input->SIZE; idx++) {
                    sprintf(metric_name, "METRIC_%ld", idx);
                    initMetricsFromArray(garrays_std_deg + idx, metric_name, deg_metrics+idx);
                }

                strcpy(metric_file_name_buf, data_file_name_buf);
                strcat(metric_file_name_buf, "_metrics");
                saveDataMETRICS(metric_file_name_buf, &deg_metrics_array);
                for (size_t idx = 0; idx < garray_result_input->SIZE; idx++) {
                    destroyMetrics(deg_metrics+idx);
                }
                for (size_t result_idx = 0; result_idx < garray_result_input->SIZE; result_idx++) {
                    free(garrays_std_deg[result_idx].DATA);
                }
                free(deg_metrics_array.DATA);
                free(garrays_std_deg);

                degradation_saved = TRUE;

                break;
        }

        iter = iter->NEXT;
    }
}

T_VOID computeInterferenceDegradation(G_ARRAY *garrays_std_input, size_t num_garrays, G_ARRAY *garrays_std_deg) {
    // Change configuration to only compile the isolated victim
    CONFIG *cfg_iso = calloc(1, sizeof(CONFIG));
    cfg_iso->NUM = 1;
    cfg_iso->VICTIM_ID = INPUT_CONFIG->VICTIM_ID;
    GET_COMP_BY_IDX(INPUT_CONFIG, cfg_iso->VICTIM_ID, (const COMP **)cfg_iso->COMPS);

    computeDegradation(garrays_std_input, num_garrays, garrays_std_deg, cfg_iso);
    free(cfg_iso);
}

T_VOID computeProprietyDegradation(G_ARRAY *garrays_std_input, size_t num_garrays, G_ARRAY *garrays_std_deg) {
    // Change configuration to only compile the isolated victim
    CONFIG *cfg_mod = cloneConfig(INPUT_CONFIG);

    const COMP *comp_ptr = NULL;
    cfg_mod->NUM = 1;
    cfg_mod->VICTIM_ID = INPUT_CONFIG->VICTIM_ID;
    GET_COMP_BY_IDX(cfg_mod, cfg_mod->VICTIM_ID, &comp_ptr);
    cfg_mod->COMPS[0] = (COMP*) comp_ptr;
    
    // Remove all Mitigation proprieties that are not marked as needed
    for (size_t prop_idx = 0; prop_idx < comp_ptr->PBUFFER->NUM; prop_idx++) {
        // The refered propriety must me a mitigation and not needed
        if (!IS_MITIGATION((comp_ptr->PBUFFER->PROPS + prop_idx)->FLAGS)) continue;

        if (IS_NEDDED((comp_ptr->PBUFFER->PROPS + prop_idx)->FLAGS)) continue;

        size_t prop_idx_aux = prop_idx;
        for (; prop_idx_aux < comp_ptr->PBUFFER->NUM-1;  prop_idx_aux++) {
            memcpy(comp_ptr->PBUFFER->PROPS + prop_idx_aux, comp_ptr->PBUFFER->PROPS + prop_idx_aux + 1, sizeof(PROP));
        }
        comp_ptr->PBUFFER->NUM--;
    }
    
    computeDegradation(garrays_std_input, num_garrays, garrays_std_deg, cfg_mod);
    destroyConfig(cfg_mod);
}

T_VOID computeDegradation(G_ARRAY *garrays_std_input, size_t num_garrays, G_ARRAY *garrays_std_deg, CONFIG *cfg) {
    // Create and initialize garray_result_iso (a G_ARRAY of RESULT to store the isolation data)
    G_ARRAY garray_result_iso = {.DATA = malloc(sizeof(RESULT) * num_garrays), .SIZE = num_garrays, .TYPE = G_RESULT};

    BUILD_PROJECT(cfg);
    runBench(garray_result_iso.SIZE, garray_result_iso.DATA);

    // Since the calculateDegradation takes the result data in an array format
    G_ARRAY *garrays_std_iso = malloc(garray_result_iso.SIZE * sizeof(G_ARRAY));

    // Transform GARRAY of RESULT into GARRAY of std type 
    for (size_t result_idx = 0; result_idx < garray_result_iso.SIZE; result_idx++) {
        garrays_std_iso[result_idx].TYPE = ((RESULT *) (garray_result_iso.DATA))[result_idx].ARRAY.TYPE;
        garrays_std_iso[result_idx].SIZE = ((RESULT *) (garray_result_iso.DATA))[result_idx].ARRAY.SIZE;
        garrays_std_iso[result_idx].DATA = ((RESULT *) (garray_result_iso.DATA))[result_idx].ARRAY.DATA;
    }

    calculateDegradation(garrays_std_iso, num_garrays, garrays_std_input, num_garrays, garrays_std_deg);

    // Need to case G_ARRAY of G_DOUBLE to G_ARRAY of G_RESULT)
    // Instead of allocating more memory, simply use the one for the garray_result_iso (same type and size
    // Deallocate the previousa allocated memory for each DATA of RESULT in garray_result_iso
    for (size_t result_idx = 0; result_idx < garray_result_iso.SIZE; result_idx++) {
        free(garrays_std_iso[result_idx].DATA);
    }
    free(garrays_std_iso);
    free(garray_result_iso.DATA);
}
