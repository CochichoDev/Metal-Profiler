#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>
#include <unistd.h>

#include "api.h"
#include "global.h"
#include "optimization.h"
#include "plot.h"
#include "utils.h"
#include "bench.h"
#include "calc.h"
#include "TUI.h"

/* STATIC FUNCTION DECLARATION */
static T_VOID mapConfigToGrid(OPT_MAP *mapGrid);
static T_VOID destroyMapGrid(OPT_MAP *mapGrid);
static PARAM_GRID generateParameterGrid(OPT_MAP *mapGrid);
static T_VOID destroyParameterGrid(OPT_MAP *mapGrid, PARAM_GRID grid);
static T_VOID printParameterGrid(OPT_MAP *mapGrid, PARAM_GRID grid);
static PARAM_GRID cloneParams(OPT_MAP *mapGrid, PARAM_GRID param);
static T_VOID buildConfigFromParameterGrid(OPT_MAP *mapGrid, PARAM_GRID grid);

static T_DOUBLE objectiveMaximizeInter(OPT_MAP *mapGrid, PARAM_GRID param);
static T_DOUBLE objectiveMinimizeInterProp(OPT_MAP *mapGrid, PARAM_GRID param);

static PARAM_GRID randomSearch(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations, \
                               T_DOUBLE (*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                               const char* output);
static PARAM_GRID randomSearchNR(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations, \
                                 T_DOUBLE (*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                                 const char* output);
static PARAM_GRID simulatedAnnealing(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations, \
                                     T_DOUBLE (*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                                     const char* output);

/* TUI Callback FUNCTIONS */
static T_VOID hoverMultiOptChoice(T_NODE *button, T_VOID **data) {
    T_NODE *multiOptChoice = data[0];
    T_NODE *multiOptAlgo = data[1];

    T_NODE *buttonRandom;
    T_NODE *buttonRandomLimited;
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
        case 1: buttonRandomLimited = create_node_button((T_POSGRID) {1, 1}, 5, 1, "Random Search Improved");
            multi_add_item(multiOptAlgo, buttonRandomLimited);
            break;
    }
    
}

static T_VOID returnExitButton(T_NODE *button, T_VOID **data) {
    T_FLAG *runFlag = data[0];
    *runFlag = FALSE;
}

static T_VOID returnOKButton(T_NODE *button, T_VOID **data) {
    if (INPUT_CONFIG == NULL) {
        dprintf(OUTPUT_DESCRIPTOR, "Error: No config is selected yet\n");
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
    T_INT iterations = parseNum(textb_get_text(iterTextb));

    if (!*output || !*textb_get_text(iterTextb)) {
        dprintf(OUTPUT_DESCRIPTOR, "Error: Both iterations and output need to be specified in the textboxes\n");
        return;
    }

    T_DOUBLE (*objectiveFunc)(OPT_MAP *, PARAM_GRID) = NULL;
    PARAM_GRID (*searchAlgo)(OPT_MAP *, PARAM_GRID , size_t , T_DOUBLE (*)(OPT_MAP *, PARAM_GRID), const char*) = NULL;

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
    multi_add_item(multiOptChoices, buttonInterferance);
    multi_add_item(multiOptChoices, buttonProp);

    T_VOID **data_multi = malloc(2*sizeof(T_NODE *));
    data_multi[0] = multiOptChoices;
    data_multi[1] = multiOptAlgo;
    hook_hover(buttonInterferance, &hoverMultiOptChoice, data_multi);
    hook_hover(buttonProp, &hoverMultiOptChoice, data_multi);

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
    OUTPUT_DESCRIPTOR = term_get_descriptor(term);


    draw();
    while(loopRun) {
        event_handler();
    }

    free(data_multi);
    free(data_exit);
    exit_tui(term_attr);

    OUTPUT_DESCRIPTOR = STDOUT_FILENO;
}

/* PARAMETER HANDLING FUNCTIONS */

/*
 * mapConfigToGrid: Fills the global OPT_MAP with the correct values that
 *                  map to the INPUT_CONFIG
 * PARAMETERS:
 *                  - mapGrid: the initialized structure of OPT_MAP to be filled
 */
static T_VOID mapConfigToGrid(OPT_MAP *mapGrid) {
    mapGrid->NUM_COMP = 0;
    
    for (size_t comp_idx = 0; comp_idx < INPUT_CONFIG->NUM; comp_idx++) {
        COMP *cur_comp = NULL;
        GET_COMP_BY_IDX(INPUT_CONFIG, INPUT_CONFIG->COMPS[comp_idx]->ID, (const COMP **) &cur_comp);

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

static T_VOID printParameterGrid(OPT_MAP *mapGrid, PARAM_GRID grid) {
    for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
        for (size_t param_idx = 0; param_idx < mapGrid->PROPS_P_ROW[row_idx]; param_idx++) {
            printf("%ld\t", grid[row_idx][param_idx].cur);
        }
        printf("\n");
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

static T_VOID buildConfigFromParameterGrid(OPT_MAP *mapGrid, PARAM_GRID grid) {
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
            }

        }
    }
}

/* OBJECTIVE FUNCTIONS */
static T_DOUBLE objectiveMaximizeInter(OPT_MAP *mapGrid, PARAM_GRID param) {
    // Compile the program w/ new config
    buildConfigFromParameterGrid(mapGrid, param);
    
    // Obtain results from running with full config
    G_ARRAY garray_result_full = {.DATA = calloc(1, sizeof(RESULT)), .TYPE = G_RESULT, .SIZE = 1};
    BUILD_PROJECT(INPUT_CONFIG);
    runBench(garray_result_full.SIZE, garray_result_full.DATA);

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
        DESTROY_RESULTS(T_UINT, result);
        free(garrays_std_deg[result_idx].DATA);
    }
    free(garray_result_full.DATA);
    free(garrays_std_full);
    free(garrays_std_deg);
    free(garray_std_max_deg.DATA);
    free(garray_std_abs_max_deg.DATA);

    return degradation;
}

static T_DOUBLE objectiveMinimizeInterProp(OPT_MAP *mapGrid, PARAM_GRID param) {
    // Compile the program w/ new config
    buildConfigFromParameterGrid(mapGrid, param);
    
    // Obtain results from running with full config
    G_ARRAY garray_result_full = {.DATA = calloc(1, sizeof(RESULT)), .TYPE = G_RESULT, .SIZE = 1};
    BUILD_PROJECT(INPUT_CONFIG);
    runBench(garray_result_full.SIZE, garray_result_full.DATA);

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
    
    T_DOUBLE degradation = ((T_DOUBLE *) garray_std_abs_max_deg.DATA)[0];

    // Clean allocations and return objective
    for (size_t result_idx = 0; result_idx < garray_result_full.SIZE; result_idx++) {
        RESULT *result = ((RESULT *) garray_result_full.DATA) + result_idx;
        DESTROY_RESULTS(T_UINT, result);
        free(garrays_std_deg[result_idx].DATA);
    }
    free(garray_result_full.DATA);
    free(garrays_std_full);
    free(garrays_std_deg);
    free(garray_std_max_deg.DATA);
    free(garray_std_abs_max_deg.DATA);

    return degradation;
}

/* SEARCH ALGO FUNCTIONS */
static PARAM_GRID randomSearch(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations, \
                               T_DOUBLE (*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                               const char* output) {
    // Define variables to register max degradation and best cur_paramss
    PARAM_GRID cur_params = cloneParams(mapGrid, param);
    T_DOUBLE best = objectiveFunc(mapGrid, cur_params);
    PARAM_GRID best_params = cloneParams(mapGrid, cur_params);

    G_ARRAY garray_result_deg = {.SIZE = 1, .TYPE = G_RESULT, .DATA = malloc(sizeof(RESULT))};
    RESULT *deg_result = garray_result_deg.DATA;
    INITIALIZE_RESULTS(T_DOUBLE, deg_result, iterations, "Optimization");

    ((T_DOUBLE *) deg_result->ARRAY.DATA)[0] = best;
    // Optimization Loop
    for (size_t iter = 1; iter < iterations; iter++) {
        // Mutate cur_paramss
        for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
            for (size_t cur_params_idx = 0; cur_params_idx < mapGrid->PROPS_P_ROW[row_idx]; cur_params_idx++) {
                T_UCHAR random = uniformRandom(0, cur_params[row_idx][cur_params_idx].max);
                //printf("%d\n", random);
                cur_params[row_idx][cur_params_idx].cur = uniformRandom(0, random);
            }
        }

        // Obtain objective
        T_DOUBLE new_objective = objectiveFunc(mapGrid, cur_params);
        // Compare and replace
        if (new_objective > best) {
            destroyParameterGrid(mapGrid, best_params);
            best_params = cloneParams(mapGrid, cur_params);
            best = new_objective;
            //printf("Best: %lf\n", best);
        }
        ((T_DOUBLE *) deg_result->ARRAY.DATA)[iter] = best;
        
    }

    char output_rs[64] = "\0";
    strncpy(output_rs, output, 63-3);
    strcat(output_rs, "_rs");
    destroyParameterGrid(mapGrid, cur_params);
    saveDataRESULTS(output_rs, &garray_result_deg);
    DESTROY_RESULTS(T_DOUBLE, deg_result);
    free(garray_result_deg.DATA);

    plotScatter("optimization_rs", "optimization_rs_scatter");

    return best_params;
}

static PARAM_GRID randomSearchNR(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations, \
                                 T_DOUBLE (*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
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
    T_DOUBLE best = objectiveFunc(mapGrid, cur_params);
    PARAM_GRID best_params = cloneParams(mapGrid, cur_params);

    G_ARRAY garray_result_deg = {.SIZE = 1, .TYPE = G_RESULT, .DATA = malloc(sizeof(RESULT))};
    RESULT *deg_result = garray_result_deg.DATA;
    INITIALIZE_RESULTS(T_DOUBLE, deg_result, iterations_limiter, "Optimization");

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


    ((T_DOUBLE *) deg_result->ARRAY.DATA)[0] = best;
    // Optimization Loop
    for (size_t iter = 1; iter < iterations_limiter; iter++) {
    
        T_FLAG new_mutation = TRUE;

        // Mutate cur_paramss
        do {
            param_buffer[0] = '\0';
            for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
                for (size_t cur_params_idx = 0; cur_params_idx < mapGrid->PROPS_P_ROW[row_idx]; cur_params_idx++) {
                    T_UCHAR random = uniformRandom(0, cur_params[row_idx][cur_params_idx].max);
                    //printf("%d\n", random);
                    cur_params[row_idx][cur_params_idx].cur = uniformRandom(0, random);
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
        T_DOUBLE new_objective = objectiveFunc(mapGrid, cur_params);
        // Compare and replace
        if (new_objective > best) {
            destroyParameterGrid(mapGrid, best_params);
            best_params = cloneParams(mapGrid, cur_params);
            best = new_objective;
            //printf("Best: %lf\n", best);
        }
        ((T_DOUBLE *) deg_result->ARRAY.DATA)[iter] = best;
        
    }

    destroyParameterGrid(mapGrid, cur_params);

    char output_rsnr[64] = "\0";
    strncpy(output_rsnr, output, 63-5);
    strcat(output_rsnr, "_rs");
    saveDataRESULTS(output_rsnr, &garray_result_deg);
    DESTROY_RESULTS(T_DOUBLE, deg_result);
    free(garray_result_deg.DATA);
    free(hashes);

    plotScatter("optimization_rs", "optimization_rs_scatter");

    return best_params;
}

static PARAM_GRID simulatedAnnealing(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations, \
                                     T_DOUBLE (*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                                     const char *output) {
    // Define variables to register max degradation and best params
    T_DOUBLE best = objectiveFunc(mapGrid, param);
    PARAM_GRID best_params = cloneParams(mapGrid, param);

    T_DOUBLE cur = best;
    PARAM_GRID cur_params = cloneParams(mapGrid, param);

    G_ARRAY garray_result_deg = {.SIZE = 1, .TYPE = G_RESULT, .DATA = malloc(sizeof(RESULT))};
    RESULT *deg_result = garray_result_deg.DATA;
    INITIALIZE_RESULTS(T_DOUBLE, deg_result, iterations, "Optimization");

    T_DOUBLE temperature = 1.0f;

    ((T_DOUBLE *) deg_result->ARRAY.DATA)[0] = best;
    // Optimization Loop
    for (size_t iter = 1; iter < iterations; iter++) {
        PARAM_GRID temp_param = cloneParams(mapGrid, cur_params);
        T_DOUBLE temp_deg = cur;

        // Mutate params
        for (size_t row_idx = 0; row_idx < mapGrid->NUM_COMP; row_idx++) {
            for (size_t param_idx = 0; param_idx < mapGrid->PROPS_P_ROW[row_idx]; param_idx++) {
                T_UCHAR random = binomialRandom(param[row_idx][param_idx].max, 0.5);
                //printf("Noise %ld\n", random-param[row_idx][param_idx].max/2);
                random += param[row_idx][param_idx].max * 1.5f;
                temp_param[row_idx][param_idx].cur = (cur_params[row_idx][param_idx].cur + random) % (param[row_idx][param_idx].max + 1);
            }
        }

        // Obtain objective
        temp_deg = objectiveFunc(mapGrid, temp_param);
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
        ((T_DOUBLE *) deg_result->ARRAY.DATA)[iter] = cur;
        
    }

    char output_sa[64] = "\0";
    strncpy(output_sa, output, 63-3);
    strcat(output_sa, "_rs");
    saveDataRESULTS(output_sa, &garray_result_deg);
    DESTROY_RESULTS(T_DOUBLE, deg_result);
    free(garray_result_deg.DATA);

    plotScatter("optimization_sa", "optimization_sa_scatter");

    return best_params;
}

T_VOID optimizeConfig(PARAM_GRID (*optimizationFunc)(OPT_MAP *, PARAM_GRID, size_t, T_DOUBLE (*)(OPT_MAP *, PARAM_GRID), const char *), \
                                    T_DOUBLE (*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                                    size_t iterations, const char *output) {
    if (iterations <= 0) {
        fprintf(stderr, "Error: Number of optimization iterations needs to be more than 0\n");
        return;
        return;
    }

    OPT_MAP mapGrid;
    mapConfigToGrid(&mapGrid);

    PARAM_GRID parameter_grid = generateParameterGrid(&mapGrid);

    //printf("%lf\n", objectiveFunc(&mapGrid, parameter_grid));
    
    PARAM_GRID best_param = optimizationFunc(&mapGrid, parameter_grid, iterations, objectiveFunc, output);

    printParameterGrid(&mapGrid, best_param);

    destroyParameterGrid(&mapGrid, parameter_grid);
    destroyParameterGrid(&mapGrid, best_param);
    destroyMapGrid(&mapGrid);
}
