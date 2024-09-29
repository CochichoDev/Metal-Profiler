#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>

#include "api.h"
#include "global.h"
#include "optimization.h"
#include "plot.h"
#include "types.h"
#include "utils.h"
#include "bench.h"
#include "calc.h"
#include "TUI.h"
#include "generics.h"

/* STATIC VARIABLE DECLARATION */
static G_ARRAY *garray_result_iso = NULL;

/* STATIC FUNCTION DECLARATION */
static T_VOID mapConfigToGrid(OPT_MAP *mapGrid, CONFIG *cfg);
static T_VOID destroyMapGrid(OPT_MAP *mapGrid);
static PARAM_GRID generateParameterGrid(OPT_MAP *mapGrid);
static T_VOID destroyParameterGrid(OPT_MAP *mapGrid, PARAM_GRID grid);
static PARAM_GRID cloneParams(OPT_MAP *mapGrid, PARAM_GRID param);
static CONFIG *buildConfigFromParameterGrid(OPT_MAP *mapGrid, PARAM_GRID grid);

static G_ARRAY *objectiveMaximizeInter(OPT_MAP *mapGrid, PARAM_GRID param);
static G_ARRAY *objectiveMinimizeInterProp(OPT_MAP *mapGrid, PARAM_GRID param);
static G_ARRAY *objectiveMinimizeMemMonitoring(OPT_MAP *mapGrid, PARAM_GRID param);

static PARAM_GRID randomSearch(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations, \
                               G_ARRAY *(*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                               const char* output);
static PARAM_GRID randomSearchNR(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations, \
                                 G_ARRAY *(*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                                 const char* output);
static PARAM_GRID randomSearchNRWeighted(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations, \
                                 G_ARRAY *(*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                                 const char *output);
static PARAM_GRID simulatedAnnealing(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations, \
                                     G_ARRAY *(*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                                     const char* output);

/* TUI Callback FUNCTIONS */
static T_VOID hoverMultiOptChoice(T_NODE *button, T_VOID **data) {
    T_NODE *multiOptChoice = data[0];
    T_NODE *multiOptAlgo = data[1];

    T_NODE *buttonRandom;
    T_NODE *buttonRandomLimited;
    T_NODE *buttonRandomLimitedWeighted;
    T_NODE *buttonAnnealing;

    multi_clean_items(multiOptAlgo);

    switch (multi_get_index(multiOptChoice)) {
        case 0:
            buttonRandom = create_node_button((T_POSGRID) {1, 1}, 5, 1, "Random Search");
            buttonRandomLimited = create_node_button((T_POSGRID) {1, 1}, 5, 1, "Random Search Improved");
            buttonAnnealing = create_node_button((T_POSGRID) {1, 1}, 5, 1, "Simulated Annealing");
            multi_add_item(multiOptAlgo, buttonRandom);
            multi_add_item(multiOptAlgo, buttonRandomLimited);
            multi_add_item(multiOptAlgo, buttonAnnealing);
            break;
        case 1: 
            buttonRandomLimited = create_node_button((T_POSGRID) {1, 1}, 5, 1, "Random Search Improved");
            multi_add_item(multiOptAlgo, buttonRandomLimited);
            break;
        case 2: 
            buttonRandomLimitedWeighted = create_node_button((T_POSGRID) {1, 1}, 5, 1, "Random Weighted Search Improved");
            multi_add_item(multiOptAlgo, buttonRandomLimitedWeighted);
            break;
    }
}

static T_VOID returnExitButton(T_NODE *button, T_VOID **data) {
    T_FLAG *runFlag = data[0];
    *runFlag = FALSE;
}

static T_VOID returnOKButton(T_NODE *button, T_VOID **data) {
    if (INPUT_CONFIG == NULL) {
        fprintf(stderr, "Error: No config is selected yet\n");
        return;
    }

    T_NODE *optChoice = data[0];
    T_NODE *optAlgo = data[1];
    T_NODE *iterTextb = data[2];
    T_NODE *outTextb = data[3];

    // Parsed arguments
    T_USHORT choice = multi_get_index(optChoice);
    T_USHORT algo = multi_get_index(optAlgo);
    const char *output = textb_get_text(outTextb);
    T_INT iterations = cliParseNum(textb_get_text(iterTextb));

    if (!*output || !*textb_get_text(iterTextb)) {
        fprintf(stderr, "Error: Both iterations and output need to be specified in the textboxes\n");
        return;
    }

    G_ARRAY *(*objectiveFunc)(OPT_MAP *, PARAM_GRID) = NULL;
    PARAM_GRID (*searchAlgo)(OPT_MAP *, PARAM_GRID , size_t , G_ARRAY *(*)(OPT_MAP *, PARAM_GRID), const char*) = NULL;

    switch (choice) {
        case 0:
            objectiveFunc = &objectiveMaximizeInter;

            switch (algo) {
                case 0:
                    searchAlgo = randomSearch;
                    break;
                case 1:
                    searchAlgo = randomSearchNR;
                    break;
                case 2:
                    searchAlgo = simulatedAnnealing;
            }

            break;
        case 1:
            objectiveFunc = &objectiveMinimizeInterProp;

            switch (algo) {
                case 0:
                    searchAlgo = randomSearchNR;
            }
            break;
        case 2:
            objectiveFunc = &objectiveMinimizeMemMonitoring;
            switch (algo) {
                case 0:
                    searchAlgo = randomSearchNRWeighted;
            }
    }

    optimizeConfig(searchAlgo, objectiveFunc, iterations, output);
}

T_VOID optimizationTUI() {
    T_FLAG loopRun = TRUE;
    T_VOID *term_attr = init_tui();
    T_NODE *root = &schema;

    /* MULTINODE CHOICES */
    T_NODE *multiOptChoices = create_node_multi((T_POSGRID) {1, 1}, WINDOW_WIDTH()/3-2, 14);
    add_node(root, multiOptChoices);

    /* MULTINODE ALGO */
    T_NODE *multiOptAlgo = create_node_multi( (T_POSGRID) {1+WINDOW_WIDTH()/3, 1}, WINDOW_WIDTH()/3-2, 14);
    add_node(root, multiOptAlgo);

    /* MULTINODE CHOICE ITEMS */
    T_NODE *buttonInterferance = create_node_button((T_POSGRID) {1, 1}, 5, 1, "Interference Maximize");
    T_NODE *buttonProp = create_node_button((T_POSGRID) {1, 1}, 5, 1, "Interference Prop Minimize");
    T_NODE *buttonMemMonitoring = create_node_button((T_POSGRID) {1, 1}, 5, 1, "Interference MemMonitoring Minimize");
    multi_add_item(multiOptChoices, buttonInterferance);
    multi_add_item(multiOptChoices, buttonProp);
    multi_add_item(multiOptChoices, buttonMemMonitoring);

    T_VOID **data_multi = malloc(2*sizeof(T_NODE *));
    data_multi[0] = multiOptChoices;
    data_multi[1] = multiOptAlgo;
    hook_hover(buttonInterferance, &hoverMultiOptChoice, data_multi);
    hook_hover(buttonProp, &hoverMultiOptChoice, data_multi);
    hook_hover(buttonMemMonitoring, &hoverMultiOptChoice, data_multi);

    /* ITERAATION TEXTBOX */
    T_NODE *textbIterations = create_node_textb( (T_POSGRID) {1+2*WINDOW_WIDTH()/3+4, 1}, WINDOW_WIDTH()/3-10);    
    add_node(root, textbIterations);

    /* OUTPUT TEXTBOX */
    T_NODE *textbOutput = create_node_textb( (T_POSGRID) {1+2*WINDOW_WIDTH()/3+4, 4}, WINDOW_WIDTH()/3-10);    
    add_node(textbIterations, textbOutput);

    /* OKAY BUTTON*/
    T_NODE *buttonOK = create_node_button( (T_POSGRID) {1+2*WINDOW_WIDTH()/3+2, 7}, WINDOW_WIDTH()/3-6, 3, "OK");
    add_node(textbOutput, buttonOK);

    T_VOID **data_okay = malloc(4*sizeof(T_VOID*));
    data_okay[0] = multiOptChoices;
    data_okay[1] = multiOptAlgo;
    data_okay[2] = textbIterations;
    data_okay[3] = textbOutput;
    hook_return(buttonOK, returnOKButton, data_okay);

    /* EXIT BUTTON */
    T_NODE *buttonExit = create_node_button( (T_POSGRID) {1+2*WINDOW_WIDTH()/3+2, 12}, WINDOW_WIDTH()/3-6, 3, "EXIT");
    add_node(buttonOK, buttonExit);

    T_VOID **data_exit = malloc(sizeof(T_FLAG*));
    data_exit[0] = &loopRun;
    hook_return(buttonExit, returnExitButton, data_exit);

    /* TERMINAL */
    T_NODE *term = create_node_term((T_POSGRID) {1, 17}, WINDOW_WIDTH()-2, WINDOW_HEIGHT()-19);
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
    free(data_multi);
    free(data_exit);
    free(data_okay);

    close(replaced_stdout);
    dup(old_stdout);
    close(replaced_stderr);
    dup(old_stderr);
    close(old_stdout);
    close(old_stderr);
}

/* PARAMETER HANDLING FUNCTIONS */

/*
 * mapConfigToGrid: Fills the global OPT_MAP with the correct values that
 *                  map to the INPUT_CONFIG
 * PARAMETERS:
 *                  - mapGrid: the initialized structure of OPT_MAP to be filled
 */
static T_VOID mapConfigToGrid(OPT_MAP *mapGrid, CONFIG *cfg) {
    assert(cfg != NULL);

    mapGrid->CFG = cfg;
    mapGrid->NUM_COMP = 0;
    
    for (size_t comp_idx = 0; comp_idx < cfg->NUM; comp_idx++) {
        COMP *cur_comp = NULL;
        GET_COMP_BY_ID(cfg, cfg->COMPS[comp_idx]->ID, (const COMP **) &cur_comp);

        T_FLAG optimizable_comp = FALSE;

        T_UCHAR comp_entry_idx;

        for (size_t prop_idx = 0; prop_idx < cur_comp->PBUFFER->NUM; prop_idx++) {
            PROP *cur_prop = cur_comp->PBUFFER->PROPS + prop_idx;

            if (!IS_OPTIMIZABLE(cur_prop->FLAGS)) continue;

            if (!optimizable_comp) {
                comp_entry_idx = mapGrid->NUM_COMP;
                mapGrid->ROWS[mapGrid->NUM_COMP++] = cur_comp;

                mapGrid->PROPS_P_ROW[comp_entry_idx] = 0;
                mapGrid->PROPS[comp_entry_idx] = NULL;

                optimizable_comp = TRUE;
            }

            T_UCHAR prop_entry_idx = mapGrid->PROPS_P_ROW[comp_entry_idx];

            mapGrid->PROPS_P_ROW[comp_entry_idx]++;


            // This is a pointer into an array in which, each element points to an array of pointers for PROP's
            PROP ***prop_map_entry = mapGrid->PROPS + comp_entry_idx;

            // Make space for a new propriety pointer
            *prop_map_entry = realloc(*prop_map_entry, sizeof(PROP *) * (prop_entry_idx+1));

            // Point new propriety pointer to the new discovered optimizable propriety
            (*prop_map_entry)[prop_entry_idx] = cur_prop;

        }

    }
}

static T_VOID destroyMapGrid(OPT_MAP *mapGrid) {
    for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
        free(*(mapGrid->PROPS + row_idx));
    }
}

static PARAM_GRID generateParameterGrid(OPT_MAP *mapGrid) {
    PARAM_GRID grid = malloc(sizeof(PARAM_ROW *) * mapGrid->NUM_COMP);


    for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
        grid[row_idx] = malloc(sizeof(PARAM_ELEM) * mapGrid->PROPS_P_ROW[row_idx]);


        for (size_t param_idx = 0; param_idx < mapGrid->PROPS_P_ROW[row_idx]; param_idx++) {
            PROP *cur_prop = mapGrid->PROPS[row_idx][param_idx];

            switch (cur_prop->PTYPE) {
                case pINT:
                case pCHAR:
                    grid[row_idx][param_idx].max = (cur_prop->iRANGE[1] - cur_prop->iRANGE[0]) / cur_prop->iSTEP;
                    grid[row_idx][param_idx].cur = (cur_prop->iINIT - cur_prop->iRANGE[0]) / cur_prop->iSTEP;
                    break;
                case pDOUBLE:
                    grid[row_idx][param_idx].max = (cur_prop->fRANGE[1] - cur_prop->fRANGE[0]) / cur_prop->fSTEP;
                    grid[row_idx][param_idx].cur = (cur_prop->fINIT - cur_prop->fRANGE[0]) / cur_prop->fSTEP;
                    break;
                case pSTR:
                    grid[row_idx][param_idx].max = strProprietyIdxByPtr(cur_prop->OPTS, NULL) - 1;
                    grid[row_idx][param_idx].cur = strProprietyIdxByValue(cur_prop->OPTS, cur_prop->sINIT);
                    break;
                default:
                    break;
            }

        }
    }

    return grid;
}

static T_VOID destroyParameterGrid(OPT_MAP *mapGrid, PARAM_GRID grid) {
    for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
        free(grid[row_idx]);
    }
    free(grid);
}

T_VOID printParameterGrid(T_INT descriptor, OPT_MAP *mapGrid, PARAM_GRID grid) {
    for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
        for (size_t param_idx = 0; param_idx < mapGrid->PROPS_P_ROW[row_idx]; param_idx++) {
            dprintf(descriptor, "%12ld\t", grid[row_idx][param_idx].cur);
        }
        dprintf(descriptor, "\n");
    }
}

T_VOID printParameterGridFILE(FILE *file, OPT_MAP *mapGrid, PARAM_GRID grid) {
    for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
        for (size_t param_idx = 0; param_idx < mapGrid->PROPS_P_ROW[row_idx]; param_idx++) {
            fprintf(file, "%12ld", grid[row_idx][param_idx].cur);
        }
    }
}

static PARAM_GRID cloneParams(OPT_MAP *mapGrid, PARAM_GRID param) {
    PARAM_GRID clone = malloc(sizeof(PARAM_ROW) * mapGrid->NUM_COMP);

    for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
        clone[row_idx] = malloc(sizeof(PARAM_ELEM) * mapGrid->PROPS_P_ROW[row_idx]);
        memcpy(clone[row_idx], param[row_idx], sizeof(PARAM_ELEM) * mapGrid->PROPS_P_ROW[row_idx]);
    }

    return clone;
}

static CONFIG *buildConfigFromParameterGrid(OPT_MAP *mapGrid, PARAM_GRID grid) {
    for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
        for (size_t param_idx = 0; param_idx < mapGrid->PROPS_P_ROW[row_idx]; param_idx++) {
            PROP *cur_prop = mapGrid->PROPS[row_idx][param_idx];

            switch (cur_prop->PTYPE) {
                case pINT:
                case pCHAR:
                    cur_prop->iINIT = grid[row_idx][param_idx].cur * cur_prop->iSTEP + cur_prop->iRANGE[0];
                    break;
                case pDOUBLE:
                    cur_prop->fINIT = grid[row_idx][param_idx].cur * cur_prop->fSTEP + cur_prop->fRANGE[0];
                    break;
                case pSTR:
                    memcpy(cur_prop->sINIT, cur_prop->OPTS[grid[row_idx][param_idx].cur],   \
                           strlen(cur_prop->OPTS[grid[row_idx][param_idx].cur]) + 1);
                    break;
                default:
                    break;
            }

        }
    }
    return mapGrid->CFG;
}

/* OBJECTIVE FUNCTIONS */
static G_ARRAY *objectiveMaximizeInter(OPT_MAP *mapGrid, PARAM_GRID param) {
    // Compile the program w/ new config
    buildConfigFromParameterGrid(mapGrid, param);
    
    // Obtain results from running with full config
    G_ARRAY garray_result_full = {.DATA = calloc(1, sizeof(RESULT)), .TYPE = G_RESULT, .SIZE = 1};
    BUILD_PROJECT(INPUT_CONFIG);
    printParameterGrid(STDOUT_FILENO, mapGrid, param);
    runBench(garray_result_full.SIZE, 1, (RESULT *[]) {garray_result_full.DATA});

    // Calculate vector of degradation
    G_ARRAY *garrays_std_full = calloc(garray_result_full.SIZE, sizeof(G_ARRAY));
    G_ARRAY *garrays_std_deg = calloc(garray_result_full.SIZE, sizeof(G_ARRAY));

    for (size_t result_idx = 0; result_idx < garray_result_full.SIZE; result_idx++) {
        RESULT *result = ((RESULT *) garray_result_full.DATA) + result_idx;
        garrays_std_full[result_idx].DATA = result->ARRAY.DATA;
        garrays_std_full[result_idx].SIZE = result->ARRAY.SIZE;
        garrays_std_full[result_idx].TYPE = result->ARRAY.TYPE;

        garrays_std_deg[result_idx].DATA = calloc(result->ARRAY.SIZE, sizeof(T_DOUBLE));
        garrays_std_deg[result_idx].SIZE = result->ARRAY.SIZE;
        garrays_std_deg[result_idx].TYPE = G_DOUBLE;
    }

    computeInterferenceDegradation(garrays_std_full, garray_result_full.SIZE, garrays_std_deg);

    // Find the maximum of the vector of degradation -> OBJECTIVE
    G_ARRAY garray_std_max_deg = {.SIZE = garray_result_full.SIZE, .TYPE = G_DOUBLE, .DATA = calloc(garray_result_full.SIZE, sizeof(T_DOUBLE))};    
    calcMaxFromArray(garrays_std_deg, garray_result_full.SIZE, &garray_std_max_deg);

    G_ARRAY garray_std_abs_max_deg = {.SIZE = 1, .TYPE = G_DOUBLE, .DATA = malloc(sizeof(T_DOUBLE))};
    calcMaxFromArray(&garray_std_max_deg, 1, &garray_std_abs_max_deg);
    
    T_DOUBLE degradation = ((T_DOUBLE *) garray_std_abs_max_deg.DATA)[0];

    // Clean allocations and return objective
    for (size_t result_idx = 0; result_idx < garray_result_full.SIZE; result_idx++) {
        RESULT *result = ((RESULT *) garray_result_full.DATA) + result_idx;
        DESTROY_RESULTS(result);
        free(garrays_std_deg[result_idx].DATA);
    }
    free(garray_result_full.DATA);
    free(garrays_std_full);
    free(garrays_std_deg);
    free(garray_std_max_deg.DATA);
    free(garray_std_abs_max_deg.DATA);

    G_ARRAY *result = calloc(1, sizeof(G_ARRAY));
    result->DATA = malloc(sizeof(T_DOUBLE));
    ((T_DOUBLE *)result->DATA)[0] = degradation;
    result->SIZE = 1;
    result->TYPE = G_DOUBLE;
    return result;
}

static G_ARRAY *objectiveMinimizeInterProp(OPT_MAP *mapGrid, PARAM_GRID param) {
    // Compile the program w/ new config
    buildConfigFromParameterGrid(mapGrid, param);
    
    // Obtain results from running with full config
    G_ARRAY garray_result_full = {.DATA = calloc(1, sizeof(RESULT)), .TYPE = G_RESULT, .SIZE = 1};
    BUILD_PROJECT(INPUT_CONFIG);
    printParameterGrid(STDOUT_FILENO, mapGrid, param);
    runBench(garray_result_full.SIZE, 1, (RESULT *[]) {garray_result_full.DATA});

    // Calculate vector of degradation
    G_ARRAY *garrays_std_full = calloc(garray_result_full.SIZE, sizeof(G_ARRAY));
    G_ARRAY *garrays_std_deg = calloc(garray_result_full.SIZE, sizeof(G_ARRAY));

    for (size_t result_idx = 0; result_idx < garray_result_full.SIZE; result_idx++) {
        RESULT *result = ((RESULT *) garray_result_full.DATA) + result_idx;
        garrays_std_full[result_idx].DATA = result->ARRAY.DATA;
        garrays_std_full[result_idx].SIZE = result->ARRAY.SIZE;
        garrays_std_full[result_idx].TYPE = result->ARRAY.TYPE;

        garrays_std_deg[result_idx].DATA = calloc(result->ARRAY.SIZE, sizeof(T_DOUBLE));
        garrays_std_deg[result_idx].SIZE = result->ARRAY.SIZE;
        garrays_std_deg[result_idx].TYPE = G_DOUBLE;
    }


    computeProprietyDegradation(garrays_std_full, garray_result_full.SIZE, garrays_std_deg);

    // Find the maximum of the vector of degradation -> OBJECTIVE
    G_ARRAY garray_std_max_deg = {.SIZE = garray_result_full.SIZE, .TYPE = G_DOUBLE, .DATA = calloc(garray_result_full.SIZE, sizeof(T_DOUBLE))};    
    calcMaxFromArray(garrays_std_deg, garray_result_full.SIZE, &garray_std_max_deg);

    G_ARRAY garray_std_abs_max_deg = {.SIZE = 1, .TYPE = G_DOUBLE, .DATA = malloc(sizeof(T_DOUBLE))};
    calcMaxFromArray(&garray_std_max_deg, 1, &garray_std_abs_max_deg);
    
    T_DOUBLE degradation = -((T_DOUBLE *) garray_std_abs_max_deg.DATA)[0];

    // Clean allocations and return objective
    for (size_t result_idx = 0; result_idx < garray_result_full.SIZE; result_idx++) {
        RESULT *result = ((RESULT *) garray_result_full.DATA) + result_idx;
        DESTROY_RESULTS(result);
        free(garrays_std_deg[result_idx].DATA);
    }
    free(garray_result_full.DATA);
    free(garrays_std_full);
    free(garrays_std_deg);
    free(garray_std_max_deg.DATA);
    free(garray_std_abs_max_deg.DATA);

    G_ARRAY *result = calloc(1, sizeof(G_ARRAY));
    result->DATA = malloc(sizeof(T_DOUBLE));
    ((T_DOUBLE *)result->DATA)[0] = degradation;
    result->SIZE = 1;
    result->TYPE = G_DOUBLE;
    return result;
}

static G_ARRAY *objectiveMinimizeMemMonitoring(OPT_MAP *mapGrid, PARAM_GRID param) {
    static T_DOUBLE avg1 = 0, avg2 = 0;
    static T_DOUBLE num1 = 0, num2 = 0;

    if (mapGrid == NULL || param == NULL) {
        avg1 = 0, avg2 = 0;
        num1 = 0, num2 = 0;
    }


    // Compile the program w/ new config (MemMonitoring and Attackers)
    CONFIG *full_cfg = buildConfigFromParameterGrid(mapGrid, param);
    
    // Obtain results from running with full config
    G_ARRAY garray_result_full = {.DATA = calloc(1, sizeof(RESULT)), .TYPE = G_RESULT, .SIZE = 1};
    G_ARRAY garray_result_mm_iso = {.DATA = calloc(1, sizeof(RESULT)), .TYPE = G_RESULT, .SIZE = 1};
    BUILD_PROJECT(full_cfg);
    printParameterGrid(STDOUT_FILENO, mapGrid, param);
    runBench(garray_result_full.SIZE, 1, (RESULT *[]) {garray_result_full.DATA});


    // Obtain results from running w/ MemMonitoring but isolation
    CONFIG *cfg_mm_iso = calloc(1, sizeof(CONFIG));
    cfg_mm_iso->NUM = 2;
    cfg_mm_iso->VICTIM_ID = full_cfg->VICTIM_ID;
    GET_COMP_BY_ID(full_cfg, SYSTEM_COMP_ID, (const COMP **)cfg_mm_iso->COMPS);
    GET_COMP_BY_ID(full_cfg, cfg_mm_iso->VICTIM_ID, (const COMP **)(cfg_mm_iso->COMPS)+1);
    BUILD_PROJECT(cfg_mm_iso);
    runBench(garray_result_mm_iso.SIZE, 1, (RESULT *[]) {garray_result_mm_iso.DATA});

    T_DOUBLE *deg1 = calculateDegradationNormalized(&garray_result_mm_iso, &garray_result_full, avg1, num1);
    avg1 = deg1[1];
    num1++;

    // This should always be true the first time except whenever it's ran for the first time
    if (garray_result_iso == NULL) {
        // Change configuration to only compile the isolated victim wo/ MemMonitoring
        CONFIG *cfg_iso = cloneConfig(cfg_mm_iso);

        COMP *sys_comp;
        if (GET_COMP_BY_ID(cfg_iso, SYSTEM_COMP_ID, (const COMP **)&sys_comp) == -1) {
            fprintf(stderr, "Error: Could not obtain base configuration because system component is not present\n");
            return NULL;
        }

        size_t mon_prop_idx;
        if ((mon_prop_idx = GET_PROP_BY_NAME(sys_comp, "MEMBANDWIDTH", NULL)) != -1) {
            assert(sys_comp->PBUFFER->PROPS[mon_prop_idx].PTYPE == pBOOL);
            sys_comp->PBUFFER->PROPS[mon_prop_idx].iINIT = 0;
        }

        garray_result_iso = calloc(1, sizeof(G_ARRAY));
        garray_result_iso->DATA = malloc(sizeof(RESULT) * NUM_ISO_RESULTS);
        garray_result_iso->SIZE = NUM_ISO_RESULTS;
        garray_result_iso->TYPE = G_RESULT;

        BUILD_PROJECT(cfg_iso);
        runBench(garray_result_iso->SIZE, 1, (RESULT *[]){garray_result_iso->DATA});
        destroyConfig(cfg_iso);
    }
    free(cfg_mm_iso);
    // Repeat the same calculations for the second metric (Influnce of MemMonitoring)
    T_DOUBLE *deg2 = calculateDegradationNormalized(garray_result_iso, &garray_result_mm_iso, avg2, num2);
    avg2 = deg2[1];
    num2++;

    
    G_ARRAY *results = calloc(1, sizeof(G_ARRAY));
    results->DATA = malloc(4*sizeof(T_DOUBLE));
    results->SIZE = 4;
    results->TYPE = G_DOUBLE;
    ((T_DOUBLE *)results->DATA)[0] = deg1[0];
    ((T_DOUBLE *)results->DATA)[1] = avg1;
    ((T_DOUBLE *)results->DATA)[2] = deg2[0];
    ((T_DOUBLE *)results->DATA)[3] = avg2;

    // Clean allocations and return objective
    for (size_t result_idx = 0; result_idx < garray_result_full.SIZE; result_idx++) {
        RESULT *result = ((RESULT *) garray_result_full.DATA) + result_idx;
        DESTROY_RESULTS(result);
        result = ((RESULT *) garray_result_mm_iso.DATA) + result_idx;
        DESTROY_RESULTS(result);
    }

    free(garray_result_full.DATA);
    free(garray_result_mm_iso.DATA);
    free(deg1);
    free(deg2);

    return results;
}

/* SEARCH ALGO FUNCTIONS */
static PARAM_GRID randomSearch(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations, \
                               G_ARRAY *(*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                               const char* output) {

    // Define variables to register max degradation and best cur_paramss
    PARAM_GRID cur_params = cloneParams(mapGrid, param);
    G_ARRAY *result_array = objectiveFunc(mapGrid, param);
    assert(result_array->TYPE == G_DOUBLE);
    T_DOUBLE best = ((T_DOUBLE *)result_array->DATA)[0];
    DESTROY_GENERIC(result_array);
    free(result_array);
    PARAM_GRID best_params = cloneParams(mapGrid, cur_params);

    G_ARRAY garray_opt_result = {.SIZE = iterations, .TYPE = G_OPTRESULT, .DATA = malloc(sizeof(OPT_RESULT) * iterations)};

    ((OPT_RESULT *) garray_opt_result.DATA)[0].DEG = best;
    ((OPT_RESULT *) garray_opt_result.DATA)[0].GRID = cloneParams(mapGrid, best_params);
    // Optimization Loop
    for (size_t iter = 1; iter < iterations; iter++) {
        // Mutate cur_paramss
        for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
            for (size_t cur_params_idx = 0; cur_params_idx < mapGrid->PROPS_P_ROW[row_idx]; cur_params_idx++) {
                T_INT random = uniformRandom(0, cur_params[row_idx][cur_params_idx].max);
                //fprintf(stdout, "%d\n", random);
                cur_params[row_idx][cur_params_idx].cur = random;

            }
        }


        // Obtain objective
        result_array = objectiveFunc(mapGrid, cur_params);
        assert(result_array->TYPE == G_DOUBLE);
        T_DOUBLE new_objective = ((T_DOUBLE *)result_array->DATA)[0];
        DESTROY_GENERIC(result_array);
        free(result_array);
        printf("ITERATION: %ld\n", iter);
        // Compare and replace
        if (new_objective > best) {
            destroyParameterGrid(mapGrid, best_params);
            best_params = cloneParams(mapGrid, cur_params);
            best = new_objective;
            //printf("Best: %lf\n", best);
        }
        ((OPT_RESULT *) garray_opt_result.DATA)[iter].DEG = best;
        ((OPT_RESULT *) garray_opt_result.DATA)[iter].GRID = cloneParams(mapGrid, best_params);
        
    }

    char output_rs[64] = "\0";
    strncpy(output_rs, output, 63-3);
    strcat(output_rs, "_rs");
    destroyParameterGrid(mapGrid, cur_params);
    saveDataOptimizationResults(output_rs, &garray_opt_result, mapGrid);
    DESTROY_GENERIC(&garray_opt_result);

    char output_rs_scatter[128] = "\0";
    strncpy(output_rs_scatter, output_rs, 64);
    strcat(output_rs_scatter, "_scatter");
    plotScatter(output_rs, output_rs_scatter);

    return best_params;
}

static PARAM_GRID randomSearchNR(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations, \
                                 G_ARRAY *(*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                                 const char *output) {
    /*
     * Since every iteration we get for sure a new configuration then it
     * makes sense to limit the number of iterations to the maximum different
     * configurations one can have
     */
    size_t num_max_states = 1;
    for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
        for (size_t cur_params_idx = 0; cur_params_idx < mapGrid->PROPS_P_ROW[row_idx]; cur_params_idx++) {
            num_max_states *= (param[row_idx][cur_params_idx].max + 1);
        }
    }
    size_t iterations_limiter = ( (iterations < num_max_states) ? iterations : num_max_states );

    // Define variables to register max degradation and best cur_paramss
    PARAM_GRID cur_params = cloneParams(mapGrid, param);
    G_ARRAY *result_array = objectiveFunc(mapGrid, param);
    assert(result_array->TYPE == G_DOUBLE);
    T_DOUBLE best = ((T_DOUBLE *)result_array->DATA)[0];
    DESTROY_GENERIC(result_array);
    free(result_array);
    PARAM_GRID best_params = cloneParams(mapGrid, cur_params);

    G_ARRAY garray_opt_result = {.SIZE = iterations_limiter, .TYPE = G_OPTRESULT, .DATA = malloc(sizeof(OPT_RESULT) * iterations)};

    T_UCHAR (*hashes)[32] = calloc(iterations_limiter, sizeof(T_UCHAR[32]));
    T_CHAR param_buffer[1024] = "\0";
    T_CHAR num_buffer[16];
    T_UCHAR new_hash[32];

    // Save initial configuration's hash
    for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
        for (size_t cur_params_idx = 0; cur_params_idx < mapGrid->PROPS_P_ROW[row_idx]; cur_params_idx++) {
            itos(cur_params[row_idx][cur_params_idx].cur, num_buffer);
            strcat(param_buffer, num_buffer);
        }
    }
    SHA256((const unsigned char *)param_buffer, strlen(param_buffer), hashes[0]);



    ((OPT_RESULT *) garray_opt_result.DATA)[0].DEG = best;
    ((OPT_RESULT *) garray_opt_result.DATA)[0].GRID = cloneParams(mapGrid, best_params);
    // Optimization Loop
    for (size_t iter = 1; iter < iterations_limiter; iter++) {
    
        T_FLAG new_mutation = TRUE;

        // Mutate cur_paramss
        do {
            param_buffer[0] = '\0';
            for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
                for (size_t cur_params_idx = 0; cur_params_idx < mapGrid->PROPS_P_ROW[row_idx]; cur_params_idx++) {
                    T_INT random = uniformRandom(0, cur_params[row_idx][cur_params_idx].max);
                    //printf("%d\n", random);
                    cur_params[row_idx][cur_params_idx].cur = random;
                    itos(cur_params[row_idx][cur_params_idx].cur, num_buffer);
                    strcat(param_buffer, num_buffer);
                }
            }
            SHA256((const unsigned char *)param_buffer, strlen(param_buffer), new_hash);

            new_mutation = TRUE;
            for (size_t hash_idx = 0; hash_idx < iter; hash_idx++) {
                uint64_t *hash64_entry = (uint64_t *) hashes[hash_idx];
                if (*hash64_entry == *(uint64_t *)new_hash && \
                    *(hash64_entry + 1) == *((uint64_t *)new_hash + 1) && \
                    *(hash64_entry + 2) == *((uint64_t *)new_hash + 2) && \
                    *(hash64_entry + 3) == *((uint64_t *)new_hash + 3))
                {
                    new_mutation = FALSE;
                    break;
                }
            }
        } while (!new_mutation);

        uint64_t *hash64_entry = (uint64_t *) hashes[iter];
        *hash64_entry = *(uint64_t *)new_hash;
        *(hash64_entry + 1) = *((uint64_t *)new_hash + 1);
        *(hash64_entry + 2) = *((uint64_t *)new_hash + 2);
        *(hash64_entry + 3) = *((uint64_t *)new_hash + 3);


        // Obtain objective
        result_array = objectiveFunc(mapGrid, cur_params);
        assert(result_array->TYPE == G_DOUBLE);
        T_DOUBLE new_objective = ((T_DOUBLE *)result_array->DATA)[0];
        DESTROY_GENERIC(result_array);
        free(result_array);
        printf("ITERATION: %ld\n", iter);
        // Compare and replace
        if (new_objective > best) {
            destroyParameterGrid(mapGrid, best_params);
            best_params = cloneParams(mapGrid, cur_params);
            best = new_objective;
            //printf("Best: %lf\n", best);
        }
        ((OPT_RESULT *) garray_opt_result.DATA)[iter].DEG = best;
        ((OPT_RESULT *) garray_opt_result.DATA)[iter].GRID = cloneParams(mapGrid, best_params);
        
    }

    destroyParameterGrid(mapGrid, cur_params);

    char output_rsnr[64] = "\0";
    strncpy(output_rsnr, output, 63-5);
    strcat(output_rsnr, "_rsnr");
    saveDataOptimizationResults(output_rsnr, &garray_opt_result, mapGrid);
    DESTROY_GENERIC(&garray_opt_result);
    free(hashes);

    char output_rsnr_scatter[128] = "\0";
    strncpy(output_rsnr_scatter, output_rsnr, 64);
    strcat(output_rsnr_scatter, "_scatter");
    plotScatter(output_rsnr, output_rsnr_scatter);

    return best_params;
}

static PARAM_GRID randomSearchNRWeighted(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations, \
                                 G_ARRAY *(*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                                 const char *output) {
    /*
     * Since every iteration we get for sure a new configuration then it
     * makes sense to limit the number of iterations to the maximum different
     * configurations one can have
     */
    size_t num_max_states = 1;
    for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
        for (size_t cur_params_idx = 0; cur_params_idx < mapGrid->PROPS_P_ROW[row_idx]; cur_params_idx++) {
            num_max_states *= (param[row_idx][cur_params_idx].max + 1);
        }
    }
    size_t iterations_limiter = ( (iterations < num_max_states) ? iterations : num_max_states );


    // Define variables to register max degradation and best cur_paramss
    PARAM_GRID cur_params = cloneParams(mapGrid, param);
    G_ARRAY *result_array = objectiveFunc(mapGrid, cur_params);
    PARAM_GRID best_params = cloneParams(mapGrid, cur_params);

    T_DOUBLE deg1 = ((T_DOUBLE *)result_array->DATA)[0], deg2 = ((T_DOUBLE *)result_array->DATA)[2];
    T_DOUBLE avg1 = ((T_DOUBLE *)result_array->DATA)[1], avg2 = ((T_DOUBLE *)result_array->DATA)[3];
    T_DOUBLE best = -(deg1 * WEIGHT1 + deg2 * WEIGHT2);
    DESTROY_GENERIC(result_array);
    free(result_array);

    G_ARRAY garray_opt_result = {.SIZE = iterations_limiter, .TYPE = G_OPTRESULT, .DATA = malloc(sizeof(OPT_RESULT) * iterations)};

    T_UCHAR (*hashes)[32] = calloc(iterations_limiter, sizeof(T_UCHAR[32]));
    T_CHAR param_buffer[1024] = "\0";
    T_CHAR num_buffer[16];
    T_UCHAR new_hash[32];

    // Save initial configuration's hash
    for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
        for (size_t cur_params_idx = 0; cur_params_idx < mapGrid->PROPS_P_ROW[row_idx]; cur_params_idx++) {
            itos(cur_params[row_idx][cur_params_idx].cur, num_buffer);
            strcat(param_buffer, num_buffer);
        }
    }
    SHA256((const unsigned char *)param_buffer, strlen(param_buffer), hashes[0]);


    ((OPT_RESULT *) garray_opt_result.DATA)[0].DEG = -best;
    ((OPT_RESULT *) garray_opt_result.DATA)[0].GRID = cloneParams(mapGrid, best_params);
    // Optimization Loop
    for (size_t iter = 1; iter < iterations_limiter; iter++) {
    
        T_FLAG new_mutation = TRUE;

        // Mutate cur_paramss
        do {
            param_buffer[0] = '\0';
            for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
                for (size_t cur_params_idx = 0; cur_params_idx < mapGrid->PROPS_P_ROW[row_idx]; cur_params_idx++) {
                    T_INT random = uniformRandom(0, cur_params[row_idx][cur_params_idx].max);
                    //printf("%d\n", random);
                    cur_params[row_idx][cur_params_idx].cur = random;
                    itos(cur_params[row_idx][cur_params_idx].cur, num_buffer);
                    strcat(param_buffer, num_buffer);
                }
            }
            // Creates hash for the parameters and makes sure it's a new combination
            SHA256((const unsigned char *)param_buffer, strlen(param_buffer), new_hash);

            new_mutation = TRUE;
            for (size_t hash_idx = 0; hash_idx < iter; hash_idx++) {
                uint64_t *hash64_entry = (uint64_t *) hashes[hash_idx];
                if (*hash64_entry == *(uint64_t *)new_hash && \
                    *(hash64_entry + 1) == *((uint64_t *)new_hash + 1) && \
                    *(hash64_entry + 2) == *((uint64_t *)new_hash + 2) && \
                    *(hash64_entry + 3) == *((uint64_t *)new_hash + 3))
                {
                    new_mutation = FALSE;
                    break;
                }
            }
        } while (!new_mutation);

        // Save new hash
        uint64_t *hash64_entry = (uint64_t *) hashes[iter];
        *hash64_entry = *(uint64_t *)new_hash;
        *(hash64_entry + 1) = *((uint64_t *)new_hash + 1);
        *(hash64_entry + 2) = *((uint64_t *)new_hash + 2);
        *(hash64_entry + 3) = *((uint64_t *)new_hash + 3);


        // Obtain objective
        result_array = objectiveFunc(mapGrid, cur_params);
        T_DOUBLE new_deg1 = ((T_DOUBLE *)result_array->DATA)[0], new_deg2 = ((T_DOUBLE *)result_array->DATA)[2];
        T_DOUBLE new_avg1 = ((T_DOUBLE *)result_array->DATA)[1], new_avg2 = ((T_DOUBLE *)result_array->DATA)[3];
        DESTROY_GENERIC(result_array);
        free(result_array);

        // Update best objective w/ new average
        deg1 *= (avg1/new_avg1), deg2 *= (avg2/new_avg2);
        best = -(deg1 * WEIGHT1 + deg2 * WEIGHT2);

        // Calculate new obtained objective
        T_DOUBLE new_objective = -(new_deg1 * WEIGHT1 + new_deg2 * WEIGHT2);

        // Update averages
        avg1 = new_avg1, avg2 = new_avg2;

        printf("ITERATION: %ld\n", iter);
        // Compare and replace
        if (new_objective > best) {
            destroyParameterGrid(mapGrid, best_params);
            best_params = cloneParams(mapGrid, cur_params);
            best = new_objective;
            deg1 = new_deg1, deg2 = new_deg2;
            //printf("Best: %lf\n", best);
        }
        ((OPT_RESULT *) garray_opt_result.DATA)[iter].DEG = -best;
        ((OPT_RESULT *) garray_opt_result.DATA)[iter].GRID = cloneParams(mapGrid, best_params);
        
    }

    destroyParameterGrid(mapGrid, cur_params);

    char output_rsnr[64] = "\0";
    strncpy(output_rsnr, output, 63-5);
    strcat(output_rsnr, "_rsnr");
    saveDataOptimizationResults(output_rsnr, &garray_opt_result, mapGrid);
    DESTROY_GENERIC(&garray_opt_result);
    free(hashes);

    char output_rsnr_scatter[128] = "\0";
    strncpy(output_rsnr_scatter, output_rsnr, 64);
    strcat(output_rsnr_scatter, "_scatter");
    plotScatter(output_rsnr, output_rsnr_scatter);

    return best_params;
}

static PARAM_GRID simulatedAnnealing(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations, \
                                     G_ARRAY *(*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                                     const char *output) {
    // Define variables to register max degradation and best params
    G_ARRAY *result_array = objectiveFunc(mapGrid, param);
    assert(result_array->TYPE == G_DOUBLE);
    T_DOUBLE best = ((T_DOUBLE *)result_array->DATA)[0];
    DESTROY_GENERIC(result_array);
    free(result_array);
    PARAM_GRID best_params = cloneParams(mapGrid, param);

    T_DOUBLE cur = best;
    PARAM_GRID cur_params = cloneParams(mapGrid, param);

    G_ARRAY garray_opt_result = {.SIZE = iterations, .TYPE = G_OPTRESULT, .DATA = malloc(sizeof(OPT_RESULT) * iterations)};

    T_DOUBLE temperature = 1.0f;

    ((OPT_RESULT *) garray_opt_result.DATA)[0].DEG = best;
    ((OPT_RESULT *) garray_opt_result.DATA)[0].GRID = cloneParams(mapGrid, best_params);
    // Optimization Loop
    for (size_t iter = 1; iter < iterations; iter++) {
        PARAM_GRID temp_param = cloneParams(mapGrid, cur_params);
        T_DOUBLE temp_deg = cur;

        // Mutate params
        for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
            for (size_t param_idx = 0; param_idx < mapGrid->PROPS_P_ROW[row_idx]; param_idx++) {
                T_INT random = binomialRandom(param[row_idx][param_idx].max, 0.5);
                //printf("Noise %ld\n", random-param[row_idx][param_idx].max/2);
                random += param[row_idx][param_idx].max * 1.5f;
                temp_param[row_idx][param_idx].cur = (cur_params[row_idx][param_idx].cur + random) % (param[row_idx][param_idx].max + 1);
            }
        }


        // Obtain objective
        result_array = objectiveFunc(mapGrid, temp_param);
        assert(result_array->TYPE == G_DOUBLE);
        temp_deg = ((T_DOUBLE *)result_array->DATA)[0];
        DESTROY_GENERIC(result_array);
        free(result_array);
        printf("ITERATION: %ld\n", iter);
        // Compare and replace
        T_DOUBLE dec1 = uniformRandom(1,100) * temperature / 100.0f;
        T_DOUBLE dec2 = 1.0f - ((T_DOUBLE)temp_deg/cur);        
        //printf("Dec1: %lf\tDec2: %lf\n", dec1, dec2);
        if (temp_deg > cur ||  \
            (dec1 > dec2)) {

            destroyParameterGrid(mapGrid, cur_params);
            cur_params = cloneParams(mapGrid, temp_param);
            destroyParameterGrid(mapGrid, temp_param);
            cur = temp_deg;
    
            if (cur > best) {
                best = cur;
                destroyParameterGrid(mapGrid, best_params);
                best_params = cloneParams(mapGrid, cur_params);
            }

        }
        temperature *= 0.99f;
        //printf("Cur: %lf\n", cur);
        //printf("Temperature: %lf\n", temperature);
        ((OPT_RESULT *) garray_opt_result.DATA)[iter].DEG = best;
        ((OPT_RESULT *) garray_opt_result.DATA)[iter].GRID = cloneParams(mapGrid, best_params);
        
    }

    char output_sa[64] = "\0";
    strncpy(output_sa, output, 63-3);
    strcat(output_sa, "_rs");
    saveDataOptimizationResults(output_sa, &garray_opt_result, mapGrid);
    DESTROY_GENERIC(&garray_opt_result);

    char output_sa_scatter[128] = "\0";
    strncpy(output_sa_scatter, output_sa, 64);
    strcat(output_sa_scatter, "_scatter");
    plotScatter(output_sa, output_sa_scatter);

    return best_params;
}

T_VOID optimizeConfig(PARAM_GRID (*optimizationFunc)(OPT_MAP *, PARAM_GRID, size_t, G_ARRAY *(*)(OPT_MAP *, PARAM_GRID), const char *), \
                                    G_ARRAY *(*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                                    size_t iterations, const char *output) {
    if (iterations <= 0) {
        fprintf(stderr, "Error: Number of optimization iterations needs to be more than 0\n");
        return;
        return;
    }

    OPT_MAP mapGrid;
    mapConfigToGrid(&mapGrid, INPUT_CONFIG);

    PARAM_GRID parameter_grid = generateParameterGrid(&mapGrid);

    //printf("%lf\n", objectiveFunc(&mapGrid, parameter_grid));
    
    PARAM_GRID best_param = optimizationFunc(&mapGrid, parameter_grid, iterations, objectiveFunc, output);

    FILE *result_file = fopen("final_config.txt", "w");
    printParameterGridFILE(result_file, &mapGrid, best_param);
    fclose(result_file);

    destroyParameterGrid(&mapGrid, parameter_grid);
    destroyParameterGrid(&mapGrid, best_param);
    destroyMapGrid(&mapGrid);

    if (garray_result_iso != NULL) {
        for (size_t result_idx = 0; result_idx < garray_result_iso->SIZE; result_idx++) {
            RESULT *result = ((RESULT *) garray_result_iso->DATA) + result_idx;
            DESTROY_RESULTS(result);
        }
        free(garray_result_iso->DATA);
    }
    free(garray_result_iso);
    garray_result_iso = NULL;
}
