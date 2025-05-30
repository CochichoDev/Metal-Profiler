/*
 * File: bench.c
 * BENCH EXECUTION AND PROCESSING
 * Author: Diogo Cochicho
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>

#include "bench.h"
#include "api.h"
#include "TUI.h"
#include "calc.h"
#include "global.h"
#include "utils.h"
#include "state.h"
#include "results.h"
#include "plot.h"

#define IGNORE_NUM 10

static T_VOID processResults(const char *name, G_ARRAY *garray_result_input, size_t num_outputs, OUTPUT **output_array);
static T_VOID computeDegradation(G_ARRAY *garrays_std_input, size_t num_garrays, G_ARRAY *garrays_std_deg, CONFIG *cfg);
static RESULT *create_result_from_output(OUTPUT_LIST *outputs);

/************** BENCH TUI FUNCTIONS ****************/
static T_VOID hoverMultiOutType(T_NODE *button, T_VOID **data) {
    T_NODE *multiResults = data[0];
    T_NODE *multiData = data[1];
    T_NODE *multiType = data[1];

    size_t resultOpt = multi_get_index(multiResults);
    size_t dataOpt = multi_get_index(multiData);
    size_t typeOpt = multi_get_index(multiType);

    OUTPUT_LIST *out_ptr = OUTPUT_LIST_SELECTED;
    for (size_t opt = resultOpt; opt > 0; out_ptr = out_ptr->NEXT, --opt);
    addOutputOption(OUTPUT_GRAPH_OPTIONS[typeOpt], OUTPUT_DATA_OPTIONS[dataOpt], out_ptr->OUT->NAME, out_ptr->OUT->DATA_SIZE, out_ptr->OUT->TYPE);
}

static T_VOID returnOKButton(T_NODE *button, T_VOID **data) {
    T_NODE *textbIterations = data[1];
    T_NODE *textbName = data[0];

    runExecution(cliParseNum(textb_get_text(textbIterations)), textb_get_text(textbName));
}

static T_VOID returnExitButton(T_NODE *button, T_VOID **data) {
    T_FLAG *runFlag = data[0];
    *runFlag = FALSE;
}

T_VOID analysisTUI() {
    assert(INPUT_CONFIG != NULL);

    T_FLAG loopRun = TRUE;
    T_VOID *term_attr = init_tui();
    T_NODE *root = &schema;

    /* MULTINODE RESULTS */
    T_NODE *multiResults = create_node_multi((T_POSGRID) {1, 1}, WINDOW_WIDTH()/3-2, 12);
    add_node(root, multiResults);

    /* MULTINODE OUTPUT DATA */
    T_NODE *multiOutData = create_node_multi( (T_POSGRID) {1+WINDOW_WIDTH()/3, 1}, WINDOW_WIDTH()/3-2, 12);
    add_node(root, multiOutData);

    /* MULTINODE OUTPUT TYPE */
    T_NODE *multiOutType = create_node_multi( (T_POSGRID) {1+2*WINDOW_WIDTH()/3, 1}, WINDOW_WIDTH()/3-2, 12);
    add_node(root, multiOutType);

    /* MULTINODE RESULTS ENTRIES */
    OUTPUT_LIST *output_ptr = OUTPUT_LIST_SELECTED;

    T_NODE **resultsEntries = NULL;
    size_t number_entries = 0;
    while (output_ptr != NULL) {
        resultsEntries = realloc(resultsEntries, sizeof(T_NODE*) * (++number_entries));
        resultsEntries[number_entries-1] = create_node_button((T_POSGRID) {1, 1}, 5, 1, output_ptr->OUT->NAME);

        output_ptr = output_ptr->NEXT;
        multi_add_item(multiResults, resultsEntries[number_entries-1]);
    }

    /* MULTINODE OUTPUT DATA ENTRIES */
    T_NODE **dataEntries = malloc(sizeof(T_NODE*)*NUM_OUTPUT_DATA);
    for (size_t idx = 0; idx < NUM_OUTPUT_DATA; idx++) {
        dataEntries[idx] = create_node_button((T_POSGRID) {1, 1}, 5, 1, OUTPUT_DATA_OPTIONS[idx]);
        multi_add_item(multiOutData, dataEntries[idx]);
    }

    /* MULTINODE OUTPUT TYPE ENTRIES */
    T_NODE **typeEntries = malloc(sizeof(T_NODE*)*NUM_OUTPUT_GRAPHS);
    T_VOID **data_multi_type = malloc(3*sizeof(T_NODE *));
    data_multi_type[0] = multiResults;
    data_multi_type[1] = multiOutData;
    data_multi_type[2] = multiOutType;
    for (size_t idx = 0; idx < NUM_OUTPUT_GRAPHS; idx++) {
        typeEntries[idx] = create_node_button((T_POSGRID) {1, 1}, 5, 1, OUTPUT_GRAPH_OPTIONS[idx]);
        multi_add_item(multiOutType, typeEntries[idx]);
        hook_hover(typeEntries[idx], &hoverMultiOutType, data_multi_type);
    }

    T_NODE *textbName = create_node_textb( (T_POSGRID) {1, 15}, WINDOW_WIDTH()/4-10);    
    add_node(multiResults, textbName);
    T_NODE *textbIterations = create_node_textb( (T_POSGRID) {WINDOW_WIDTH()/4-6, 15}, WINDOW_WIDTH()/4-10);    
    add_node(multiResults, textbIterations);

    T_NODE *buttonOK = create_node_button( (T_POSGRID) {WINDOW_WIDTH()/2-13, 15}, WINDOW_WIDTH()/4-10, 1, "OK");
    add_node(multiResults, buttonOK);

    T_NODE *buttonExit = create_node_button( (T_POSGRID) {3*WINDOW_WIDTH()/4-20, 15}, WINDOW_WIDTH()/4-10, 1, "EXIT");
    add_node(multiResults, buttonExit);

    T_VOID **data_exit = malloc(sizeof(T_FLAG*));
    data_exit[0] = &loopRun;
    hook_return(buttonExit, returnExitButton, data_exit);


    T_VOID **data_okay = malloc(2*sizeof(T_VOID*));
    data_okay[0] = textbName;
    data_okay[1] = textbIterations;
    hook_return(buttonOK, returnOKButton, data_okay);

    T_NODE *term = create_node_term((T_POSGRID) {1, 18}, WINDOW_WIDTH()-2, WINDOW_HEIGHT()-20);
    add_node(root, term);
    int OUTPUT_DESCRIPTOR = term_get_descriptor(term);

    int old_stdout = dup(STDOUT_FILENO);
    int old_stderr = dup(STDERR_FILENO);
    close(STDOUT_FILENO);
    int replaced_stdout = dup(OUTPUT_DESCRIPTOR);
    close(STDERR_FILENO);
    int replaced_stderr = dup(OUTPUT_DESCRIPTOR);

    draw();
    while(loopRun) {
        event_handler();
    }

    exit_tui(term_attr);
    free(data_multi_type);
    free(data_exit);
    free(data_okay);
    free(resultsEntries);
    free(dataEntries);
    free(typeEntries);

    close(replaced_stdout);
    dup(old_stdout);
    close(replaced_stderr);
    dup(old_stderr);
    close(old_stdout);
    close(old_stderr);
}


/************** BENCH RUNNING FUNCTIONS ****************/
T_VOID runExecution (size_t iter, const char *name) {
    if (!MODULE_CONFIG) {
        fprintf(stdout, "Error: No config has been selected\n");
        return;
    }

    if (!OUTPUT_LIST_SELECTED) {
        fprintf(stderr, "Error: No output selected\n");
        return;
    }

    // Count number of outputs
    size_t numberResults = 0;
    for (OUTPUT_LIST *out_ptr = OUTPUT_LIST_SELECTED; out_ptr != NULL; out_ptr = out_ptr->NEXT, ++numberResults); 

    RESULT **result_data = (RESULT **) malloc(sizeof(RESULT *) * numberResults);
    for (size_t result_idx = 0; result_idx < numberResults; result_idx++)
        result_data[result_idx] = (RESULT *) malloc(sizeof(RESULT) * iter);

    runBench(iter, numberResults, result_data);

    // processResults only accepts a G_ARRAY of type RESULT
    OUTPUT_LIST *out_ptr = OUTPUT_LIST_SELECTED;
    for (size_t result_idx = 0; result_idx < numberResults; out_ptr = out_ptr->NEXT, result_idx++) {
        G_ARRAY result_array = {.TYPE = G_RESULT, .DATA = result_data[result_idx], .SIZE = iter};

        // TODO: Generalize for the case where more than one OUTPUT format for the same RESULT Metric
        processResults(name, &result_array, 1, (OUTPUT *[]){out_ptr->OUT});
        plotResults(name, 1, (OUTPUT *[]){out_ptr->OUT});


        // The type ihere doesn't matter, free will only need the origin address
        for (size_t idx = 0; idx < iter; idx++)
            DESTROY_RESULTS(result_data[result_idx]+idx);
    }
    
    for (size_t result_idx = 0; result_idx < numberResults; result_idx++)
        free(result_data[result_idx]);
    free(result_data);
}

T_ERROR runBench(size_t iter, T_UINT numResults, RESULT **results_input) {
    RESULT *result = create_result_from_output(OUTPUT_LIST_SELECTED);
    for (size_t idx = 0; idx < iter; idx++) {
        INIT_BENCH();
        //cliPrintProgress(idx, iter);
        RUN_BENCH(result);
        if (!result) {
            fprintf(stderr, "Error: Couldn't get result data from the module\n");
        }

        size_t result_idx = 0;
        for (OUTPUT_LIST *out_ptr = OUTPUT_LIST_SELECTED; \
             out_ptr != NULL && result_idx < numResults; \
             out_ptr = out_ptr->NEXT, ++result_idx) 
        {
            switch (out_ptr->OUT->TYPE) {
                case G_INT:
                    INITIALIZE_RESULTS(T_UINT, results_input[result_idx]+idx, result[result_idx].ARRAY.SIZE - IGNORE_LIMIT, result[result_idx].NAME);
                    memcpy(results_input[result_idx][idx].ARRAY.DATA, result[result_idx].ARRAY.DATA + IGNORE_LIMIT*sizeof(T_UINT), \
                           sizeof(T_UINT)*(result[result_idx].ARRAY.SIZE - IGNORE_LIMIT));
                    break;
                case G_UINT:
                    INITIALIZE_RESULTS(T_UINT, results_input[result_idx]+idx, result[result_idx].ARRAY.SIZE - IGNORE_LIMIT, result[result_idx].NAME);
                    memcpy(results_input[result_idx][idx].ARRAY.DATA, result[result_idx].ARRAY.DATA + IGNORE_LIMIT*sizeof(T_UINT), \
                           sizeof(T_UINT)*(result[result_idx].ARRAY.SIZE - IGNORE_LIMIT));
                    break;
                case G_DOUBLE:
                    INITIALIZE_RESULTS(T_DOUBLE, results_input[result_idx]+idx, result[result_idx].ARRAY.SIZE - IGNORE_LIMIT, result[result_idx].NAME);
                    memcpy(results_input[result_idx][idx].ARRAY.DATA, result[result_idx].ARRAY.DATA + IGNORE_LIMIT*sizeof(T_DOUBLE), \
                           sizeof(T_DOUBLE)*(result[result_idx].ARRAY.SIZE - IGNORE_LIMIT));
                default:
                    break;
            }
        }
        EXIT_BENCH();
    }
    size_t result_idx = 0;
    for (OUTPUT_LIST *out_ptr = OUTPUT_LIST_SELECTED; out_ptr != NULL; out_ptr = out_ptr->NEXT, ++result_idx) {
        DESTROY_RESULTS(result + result_idx);
    }
    free(result);
    return 0;
}


/************** BENCH PROCESSING FUNCTIONS ****************/
/*
 * processResults : It performs the necessary data treatment that are specified
 *                  in the global OUTPUT_LIST and saves the data in files whose names
 *                  follow the convention.
 *
 *                  The input garray_result_input must be well initialized, it is a generic array
 *                  of RESULT elements.
 */
static T_VOID processResults(const char *name, G_ARRAY *garray_result_input, size_t num_outputs, OUTPUT **output_array) {
    if (garray_result_input->SIZE <= 0) {
        fprintf(stderr, "Error: The number of results must be positive\n");
        return;
    }

    if (mkdir(name, 0700) && errno != EEXIST) {
        fprintf(stderr, "Error: Could not create the %s directory\n", name);
        return;
    }

    T_FLAG raw_saved = FALSE;
    T_FLAG degradation_saved = FALSE;

    G_ARRAY raw_metrics_array = {.SIZE = garray_result_input->SIZE, .TYPE = G_METRICS, .DATA = NULL};
    G_ARRAY deg_metrics_array = {.SIZE = garray_result_input->SIZE, .TYPE = G_METRICS, .DATA = NULL};

    T_STR data_file_name_buf = { 0 };
    T_STR metric_file_name_buf = { 0 };

    for (size_t out_idx = 0; out_idx < num_outputs; out_idx++) {
        // Prepare the file path to include the requested directory
        strcpy(data_file_name_buf, name);
        strcat(data_file_name_buf, "/");
        strcat(data_file_name_buf, output_array[out_idx]->NAME);

        T_STR metric_name;
        switch (output_array[out_idx]->DATA_TYPE) {
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
    }
}

T_VOID computeInterferenceDegradation(G_ARRAY *garrays_std_input, size_t num_garrays, G_ARRAY *garrays_std_deg) {
    // Change configuration to only compile the isolated victim
    CONFIG *cfg_iso = cloneConfig(INPUT_CONFIG);
    cfg_iso->NUM = 2;
    cfg_iso->VICTIM_ID = INPUT_CONFIG->VICTIM_ID;
    GET_COMP_BY_ID(INPUT_CONFIG, SYSTEM_COMP_ID, (const COMP **)cfg_iso->COMPS);
    GET_COMP_BY_ID(INPUT_CONFIG, cfg_iso->VICTIM_ID, (const COMP **)(cfg_iso->COMPS)+1);

    computeDegradation(garrays_std_input, num_garrays, garrays_std_deg, cfg_iso);
    free(cfg_iso);
}

T_VOID computeProprietyDegradation(G_ARRAY *garrays_std_input, size_t num_garrays, G_ARRAY *garrays_std_deg) {
    // Change configuration to only compile the isolated victim
    CONFIG *cfg_mod = cloneConfig(INPUT_CONFIG);

    const COMP *comp_ptr = NULL;
    cfg_mod->NUM = 2;
    cfg_mod->VICTIM_ID = INPUT_CONFIG->VICTIM_ID;
    GET_COMP_BY_ID(cfg_mod, SYSTEM_COMP_ID, &comp_ptr);
    GET_COMP_BY_ID(cfg_mod, cfg_mod->VICTIM_ID, ((const COMP **)cfg_mod->COMPS)+1);
    cfg_mod->COMPS[0] = (COMP*) comp_ptr;
    
    // Remove all Mitigation proprieties that are not marked as needed
    for (size_t prop_idx = 0; prop_idx < comp_ptr->PBUFFER->NUM; prop_idx++) {
        // The refered propriety must me a mitigation and not needed
        if (!IS_MITIGATION((comp_ptr->PBUFFER->PROPS + prop_idx)->FLAGS)) continue;

        if (comp_ptr->PBUFFER->PROPS[prop_idx].PTYPE == pBOOL) {
            comp_ptr->PBUFFER->PROPS[prop_idx].iINIT = 0;
            continue;
        }

        size_t prop_idx_aux = prop_idx;
        for (; prop_idx_aux < comp_ptr->PBUFFER->NUM-1;  prop_idx_aux++) {
            memcpy(comp_ptr->PBUFFER->PROPS + prop_idx_aux, comp_ptr->PBUFFER->PROPS + prop_idx_aux + 1, sizeof(PROP));
        }
        comp_ptr->PBUFFER->NUM--;
    }
    
    computeDegradation(garrays_std_input, num_garrays, garrays_std_deg, cfg_mod);
    destroyConfig(cfg_mod);
}

static T_VOID computeDegradation(G_ARRAY *garrays_std_input, size_t num_garrays, G_ARRAY *garrays_std_deg, CONFIG *cfg) {
    // Create and initialize garray_result_iso (a G_ARRAY of RESULT to store the isolation data)
    G_ARRAY garray_result_iso = {.DATA = malloc(sizeof(RESULT) * num_garrays), .SIZE = num_garrays, .TYPE = G_RESULT};

    BUILD_PROJECT(cfg);
    runBench(garray_result_iso.SIZE, 1, (RESULT *[]){garray_result_iso.DATA});

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

/************** BENCH HELPER FUNCTIONS ****************/
static RESULT *create_result_from_output(OUTPUT_LIST *outputs) {
    RESULT *result_array = NULL;

    size_t size_of_array = 0;

    while (outputs != NULL) {
        size_of_array++;
        result_array = realloc(result_array, sizeof(RESULT) * size_of_array);
        
        switch (outputs->OUT->TYPE) {
            case G_INT:
            case G_UINT:
                INITIALIZE_RESULTS(T_UINT, result_array + size_of_array-1, outputs->OUT->DATA_SIZE, outputs->OUT->NAME);
                break;
            case G_DOUBLE:
                INITIALIZE_RESULTS(T_DOUBLE, result_array + size_of_array-1, outputs->OUT->DATA_SIZE, outputs->OUT->NAME);
                break;
            default:
                break;
        }

        outputs = outputs->NEXT;
    }

    return result_array;
}

