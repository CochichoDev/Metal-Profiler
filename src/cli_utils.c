#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "utils.h"
#include "api.h"
#include "apistate.h"
#include "cli.h"
#include "cli_utils.h"
#include "parsing.h"
#include "global.h"
#include "bench.h"
#include "plot.h"
#include "state.h"

T_ERROR listArchs(TERM *term) {
    char buf[256];
    for (size_t i = 0; i < AVAIL_ARCHS.num ; i++) {
        sprintf(buf, "[%ld]\t%s\n", i, AVAIL_ARCHS.arch[i].name); 
        write(term->out_descr, buf, strlen(buf));
    }
    
    return 0;
}

T_ERROR listConfigs(TERM *term) {
    char buf[256];
    fprintf(stdout, "Available configurations:\n");
    for (size_t i = 0; i < AVAIL_CONFIGS.num ; i++) {
        sprintf(buf, "\t[%ld]\t%s\n", i, AVAIL_CONFIGS.config[i].name); 
        write(term->out_descr, buf, strlen(buf));
    }
    
    return 0;
}

T_ERROR listState() {
    fprintf(stdout, "\n");
    fprintf(stdout, "+++++++++++++++++++++++++++++++++++++++++\n");
    if (SELECTED_ARCH.name[0] == '\0') {
        fprintf(stdout, "ARCH:\tNone\n");
        goto END;
    }

    fprintf(stdout, "ARCH:\t%s\n", SELECTED_ARCH.name);

    fprintf(stdout, "\n");
    if (AVAIL_CONFIGS.selected == -1) {
        fprintf(stdout, "CONFIG:\tNone\n");
        goto END;
    }
    fprintf(stdout, "CONFIG:\t%s\n", AVAIL_CONFIGS.config[AVAIL_CONFIGS.selected].name);

    fprintf(stdout, "\n");
    if (OUTPUT_LIST_SELECTED == NULL) {
        fprintf(stdout, "Outputs:\tNone\n");
        goto END;
    }
    fprintf(stdout, "Outputs:\n");
    listSelectedOutputOptions();

END:
    fprintf(stdout, "+++++++++++++++++++++++++++++++++++++++++\n");
    fprintf(stdout, "\n");

    return 0;
}



#define ERROR_ARCH "Error: No architecture has been selected\n"
#define ERROR_CONFIG "Error: No config has been selected\n"
#define MSG_INT0 "\n\t\tType: INT"
#define MSG_STR0 "\n\t\tType: STR"
#define MSG1 "\n\t\tMandatory: "
#define MSGOPTM "\n\t\tOptimizable: "
#define MSG_INT2 "\n\t\tMIN: "
#define MSG_INT3 "\n\t\tMAX: "
#define MSG_INT4 "\n\t\tVALUE: "
#define MSG_STR2 "\n\t\tVALUES: "
#define ERROR_TYPE_NOT_IMPLEMENTED "Error: The configuration type has not yet been implemented\n"
T_VOID printConfig(TERM *term) {
    if (SELECTED_ARCH.name[0] == '\0') {
        write(term->out_descr, ERROR_ARCH, sizeof(ERROR_ARCH));
        return;
    }

    COMP **comp_ptr = MODULE_CONFIG->COMPS;
    T_PSTR need, optimizable;
    char num[16];
    for (size_t cidx = 0 ; cidx < MODULE_CONFIG->NUM ; cidx++, comp_ptr++) {
        // Name of the component
        write(term->out_descr, (*comp_ptr)->NAME, strlen((*comp_ptr)->NAME));
        write(term->out_descr, "\nID=", 5);
        sprintf(num, "%d", (*comp_ptr)->ID);
        write(term->out_descr, num, strlen(num));

        PROP *prop_ptr = (*comp_ptr)->PBUFFER->PROPS;
        for (size_t pidx = 0 ; pidx < (*comp_ptr)->PBUFFER->NUM ; pidx++, prop_ptr++) {
            write(term->out_descr, "\n\t", 2);
            write(term->out_descr, prop_ptr->NAME, strlen(prop_ptr->NAME));
            switch (prop_ptr->PTYPE) {
                case pINT:
                    write(term->out_descr, MSG_INT0, sizeof(MSG_INT0));

                    write(term->out_descr, MSG1, sizeof(MSG1));
                    need = IS_NEDDED(prop_ptr->FLAGS) ? "YES" : "NO";
                    write(term->out_descr, need, strlen(need));

                    write(term->out_descr, MSGOPTM, sizeof(MSGOPTM));
                    optimizable = IS_OPTIMIZABLE(prop_ptr->FLAGS) ? "YES" : "NO";
                    write(term->out_descr, optimizable , strlen(optimizable ));

                    write(term->out_descr, MSG_INT2, sizeof(MSG_INT2));
                    sprintf(num, "%d", prop_ptr->iRANGE[0]);
                    write(term->out_descr, num, strlen(num));

                    write(term->out_descr, MSG_INT3, sizeof(MSG_INT3));
                    sprintf(num, "%d", prop_ptr->iRANGE[1]);
                    write(term->out_descr, num, strlen(num));

                    write(term->out_descr, MSG_INT4, sizeof(MSG_INT4));
                    sprintf(num, "%d", prop_ptr->iINIT);
                    write(term->out_descr, num, strlen(num));
                    
                    write(term->out_descr, "\n", 1);
                    break;
                case pDOUBLE:
                    write(term->out_descr, MSG_INT0, sizeof(MSG_INT0));

                    write(term->out_descr, MSG1, sizeof(MSG1));

                    need = IS_NEDDED(prop_ptr->FLAGS) ? "YES" : "NO";
                    write(term->out_descr, need, strlen(need));

                    write(term->out_descr, MSGOPTM, sizeof(MSGOPTM));
                    optimizable = IS_OPTIMIZABLE(prop_ptr->FLAGS) ? "YES" : "NO";
                    write(term->out_descr, optimizable , strlen(optimizable ));

                    write(term->out_descr, MSG_INT2, sizeof(MSG_INT2));
                    sprintf(num, "%f", prop_ptr->fRANGE[0]);
                    write(term->out_descr, num, strlen(num));

                    write(term->out_descr, MSG_INT3, sizeof(MSG_INT3));
                    sprintf(num, "%f", prop_ptr->fRANGE[1]);
                    write(term->out_descr, num, strlen(num));

                    write(term->out_descr, MSG_INT4, sizeof(MSG_INT4));
                    sprintf(num, "%f", prop_ptr->fINIT);
                    write(term->out_descr, num, strlen(num));
                    
                    write(term->out_descr, "\n", 1);
                    break;
                case pSTR:
                    write(term->out_descr, MSG_STR0, sizeof(MSG_STR0));

                    write(term->out_descr, MSG1, sizeof(MSG1));
                    need = IS_NEDDED(prop_ptr->FLAGS) ? "YES" : "NO";
                    write(term->out_descr, need, strlen(need));

                    write(term->out_descr, MSGOPTM, sizeof(MSGOPTM));
                    optimizable = IS_OPTIMIZABLE(prop_ptr->FLAGS) ? "YES" : "NO";
                    write(term->out_descr, optimizable , strlen(optimizable ));

                    write(term->out_descr, MSG_STR2, sizeof(MSG_STR2));
                    T_PSTR *ptr = prop_ptr->OPTS;
                    while (*ptr != NULL) {
                        write(term->out_descr, "\n\t\t\t", 4);
                        write(term->out_descr, *ptr, strlen(*ptr));
                        ptr++;
                    }
                    break;
                default:
                    write(term->out_descr, ERROR_TYPE_NOT_IMPLEMENTED, sizeof(ERROR_TYPE_NOT_IMPLEMENTED));
            }
        }
    }
}

#define ERROR_OPTION "Error: The selected option is not available\n"
#define SELECT_MSG "%s was successfully selected\n"
T_VOID selectArch(TERM *term, size_t choice) {
    if (choice < 0 || 
        choice >= AVAIL_ARCHS.num) 
    {
        write(term->out_descr, ERROR_OPTION, sizeof(ERROR_OPTION));
        return;
    }

    SELECTED_ARCH = AVAIL_ARCHS.arch[choice];
    char sel_msg[256];
    sprintf(sel_msg, SELECT_MSG, SELECTED_ARCH.name);
    write(term->out_descr, sel_msg, strlen(sel_msg));

    if (!(MODULE_HANDLE = dlopen(SELECTED_ARCH.path, RTLD_LAZY)))
        fprintf(stderr, "Error: Could not open handle of module (%s)\n", dlerror());

    if (!(MODULE_CONFIG = (CONFIG *) dlsym(MODULE_HANDLE, "ARCH_CONFIG")))
        fprintf(stderr, "Error: Could not access CONFIG variable (%s)\n", dlerror());

    if (!(BUILD_PROJECT = (T_VOID (*)(CONFIG *)) dlsym(MODULE_HANDLE, "BUILD_PROJECT")))
        fprintf(stderr, "Error: Could not access BUILD_PROJECT function (%s)\n", dlerror());

    if (!(INIT_BENCH = (T_VOID (*)(void)) dlsym(MODULE_HANDLE, "INIT_BENCH")))
        fprintf(stderr, "Error: Could not access INIT_BENCH function (%s)\n", dlerror());
    if (!(RUN_BENCH = (T_VOID (*)(RESULT *)) dlsym(MODULE_HANDLE, "RUN_BENCH")))
        fprintf(stderr, "Error: Could not access RUN_BENCH  function (%s)\n", dlerror());
    if (!(EXIT_BENCH = (T_VOID (*)(void)) dlsym(MODULE_HANDLE, "EXIT_BENCH")))
        fprintf(stderr, "Error: Could not access EXIT_BENCH   function (%s)\n", dlerror());

    loadAvailableConfigs();
}

T_VOID listOutputTypes() {
    fprintf(stdout, "GRAPH OPTIONS:\n");
    for (size_t idx = 0 ; idx < NUM_OUTPUT_GRAPHS ; idx++ ) {
        fprintf(stdout, "\t%s\n", OUTPUT_GRAPH_OPTIONS[idx]);
    }
    fprintf(stdout, "DATA OPTIONS:\n");
    for (size_t idx = 0 ; idx < NUM_OUTPUT_DATA ; idx++ ) {
        fprintf(stdout, "\t%s\n", OUTPUT_DATA_OPTIONS[idx]);
    }
}

/*
 * This function does not correct the config file if additional proprieties/components are specified
 * This function calls BUILD_PROJECT of the module with a CONFIG argument that does not necessarily have
 * all the proprieties in the same order as specified by the module (some uneeded by be missing)
 */
T_VOID loadConfig(TERM *term, T_UINT config_option) {
    if (SELECTED_ARCH.name[0] == '\0') {
        write(term->out_descr, ERROR_ARCH, sizeof(ERROR_ARCH));
        return;
    }
    if (config_option < 0 || 
        config_option >= AVAIL_CONFIGS.num) 
    {
        write(term->out_descr, ERROR_OPTION, sizeof(ERROR_OPTION));
        return;
    }

    AVAIL_CONFIGS.selected = config_option;
    FCONFIG SELECTED_CONFIG = AVAIL_CONFIGS.config[config_option];
    FILE *config_file;
    if (!(config_file = fopen(SELECTED_CONFIG.path, "r"))) {
        puts("Error: Could not open config file");
        return;
    }

    INPUT_CONFIG = parseConfig(config_file);
    fclose(config_file);
    if (!INPUT_CONFIG) {
        puts("Error: Could not parse input config file");
        return;
    }

    // Go over all the components parsed
    for (size_t m_comp_idx = 0 ; m_comp_idx < MODULE_CONFIG->NUM ; m_comp_idx++) {
        // Get the corresponding index of the component being analyzed base on its index
        size_t comp_idx;
        for (comp_idx = 0 ; comp_idx < INPUT_CONFIG->NUM ; comp_idx++) 
            if (MODULE_CONFIG->COMPS[m_comp_idx]->ID == INPUT_CONFIG->COMPS[comp_idx]->ID) break;

        // In case no component was found for the corresponding ID continue to search for next
        if (comp_idx == INPUT_CONFIG->NUM) continue;

        COMP *m_comp = MODULE_CONFIG->COMPS[m_comp_idx];
        COMP *comp = INPUT_CONFIG->COMPS[comp_idx];

        // Go over all the proprieties of the component in question
        for (size_t m_prop_idx = 0 ; m_prop_idx < m_comp->PBUFFER->NUM ; m_prop_idx++) {
            PROP *m_prop = m_comp->PBUFFER->PROPS + m_prop_idx;
            size_t prop_idx;
            // Get the corresponding propriety index based on propriety name
            for (prop_idx = 0 ; prop_idx < comp->PBUFFER->NUM ; prop_idx++)
                if (!strcmp(m_prop->NAME, comp->PBUFFER->PROPS[prop_idx].NAME)) break;

            // If the propriety was not defined and it's needed throw an error
            if (prop_idx == comp->PBUFFER->NUM && IS_NEDDED(m_prop->FLAGS)) {
                fprintf(stderr, "Error: Propriety %s was not defined\n", m_prop->NAME);
                goto ERROR;
            }

            PROP *prop = comp->PBUFFER->PROPS + prop_idx;

            if (m_prop->PTYPE != prop->PTYPE) {
                fprintf(stderr, "Error: Propriety %s does not have expected type %d, should have type %d\n", prop->NAME, prop->PTYPE, m_prop->PTYPE);
                goto ERROR;
            }

            if (( IS_OPTIMIZABLE(prop->FLAGS) && !IS_OPTIMIZABLE(m_prop->FLAGS) ) ||
                ( IS_MITIGATION(prop->FLAGS) && !IS_MITIGATION(m_prop->FLAGS) )) {
                fprintf(stderr, "Error: Propriety %s cannot possess the specified flag\n", prop->NAME);
                goto ERROR;
            }

            T_PSTR *opt_idx = m_prop->OPTS;
            switch (prop->PTYPE) {
                case pDOUBLE:
                    if (prop->fINIT > m_prop->fRANGE[1] || prop->fINIT < m_prop->fRANGE[0])
                        goto lRANGE_ERROR;
                    memcpy(prop->fRANGE, m_prop->fRANGE, sizeof(m_prop->fRANGE));
                    prop->fSTEP = m_prop->fSTEP;
                    break;
                case pINT:
                    if (prop->iINIT > m_prop->iRANGE[1] || prop->iINIT < m_prop->iRANGE[0])
                        goto lRANGE_ERROR;
                    memcpy(prop->iRANGE, m_prop->iRANGE, sizeof(m_prop->iRANGE));
                    prop->iSTEP = m_prop->iSTEP;
                    break;
                case pSTR:
                    while (*opt_idx != NULL) {
                        if (!strcmp(*opt_idx, prop->sINIT)) break;
                        opt_idx++;
                    }
                    if (*opt_idx == NULL) goto lRANGE_ERROR;
                    memcpy(prop->OPTS, m_prop->OPTS, sizeof(m_prop->OPTS));
                    break;
                default:
                    break;
            } 
            continue;
            lRANGE_ERROR:
                fprintf(stderr, "Error: Value of %s in component %d is out-of-bounds\n", prop->NAME, comp->ID);
                goto ERROR;
        }
    }

    BUILD_PROJECT(INPUT_CONFIG);
    return;

ERROR:
    fprintf(stderr, "Error: Input configuration %s not accepted\n", AVAIL_CONFIGS.config[AVAIL_CONFIGS.selected].name);
    INPUT_CONFIG = NULL;
    AVAIL_CONFIGS.selected = -1;
}

T_VOID runExecution (size_t iter) {
    if (!MODULE_CONFIG) {
        fprintf(stdout, ERROR_CONFIG);
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
        result_data[result_idx] = (RESULT *) malloc(sizeof(RESULT *) * iter);

    runBench(iter, numberResults, result_data);

    // processResults only accepts a G_ARRAY of type RESULT
    OUTPUT_LIST *out_ptr = OUTPUT_LIST_SELECTED;
    for (size_t result_idx = 0; result_idx < numberResults; out_ptr = out_ptr->NEXT, result_idx++) {
        G_ARRAY result_array = {.TYPE = G_RESULT, .DATA = result_data[result_idx]->ARRAY.DATA, .SIZE = iter};

        // TODO: Generalize for the case where more than one OUTPUT format for the same RESULT Metric
        processResults(&result_array, 1, (OUTPUT *[]){out_ptr->OUT});
        plotResults(1, (OUTPUT *[]){out_ptr->OUT});


        // The type ihere doesn't matter, free will only need the origin address
        for (size_t idx = 0; idx < iter; idx++)
            DESTROY_RESULTS(result_data[result_idx]+idx);
    }
    

    free(result_data);
}

