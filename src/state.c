/*
 * File: state.c
 * STATE MANAGEMENT FUNCTIONALITY
 * Author: Diogo Cochicho
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include "api.h"
#include "global.h"
#include "state.h"
#include "utils.h"

static T_UINT __getType(T_STR *type_options, size_t num_options, T_PSTR type);
static const T_PSTR __getTypeName(T_STR *type_options, size_t num_options, T_UINT type);
static CONFIG *parseConfig(FILE *fd);

/************** PRINTING STATE FUNCTIONS ****************/
T_ERROR listArchs() {
    for (size_t i = 0; i < AVAIL_ARCHS.num ; i++) {
        fprintf(stdout, "[%ld]\t%s\n", i, AVAIL_ARCHS.arch[i].name);
    }
    
    return 0;
}

T_ERROR listConfigs() {
    fprintf(stdout, "Available configurations:\n");
    for (size_t i = 0; i < AVAIL_CONFIGS.num ; i++) {
        fprintf(stdout, "\t[%ld]\t%s\n", i, AVAIL_CONFIGS.config[i].name); 
    }
    
    return 0;
}

T_VOID printConfig() {
    if (SELECTED_ARCH.name[0] == '\0') {
        fprintf(stderr, "Error: No architecture has been selected\n");
        return;
    }

    COMP **comp_ptr = MODULE_CONFIG->COMPS;
    T_PSTR need, optimizable;
    for (size_t cidx = 0 ; cidx < MODULE_CONFIG->NUM ; cidx++, comp_ptr++) {
        // Name of the component
        fprintf(stdout, "%s", (*comp_ptr)->NAME);
        fprintf(stdout, "%s", "\nID=");
        fprintf(stdout, "%d", (*comp_ptr)->ID);

        PROP *prop_ptr = (*comp_ptr)->PBUFFER->PROPS;
        for (size_t pidx = 0 ; pidx < (*comp_ptr)->PBUFFER->NUM ; pidx++, prop_ptr++) {
            fprintf(stdout, "\n\t%s", prop_ptr->NAME);
            switch (prop_ptr->PTYPE) {
                case pINT:
                    fprintf(stdout, "\n\t\tType: INT");

                    fprintf(stdout, "\n\t\tMandatory: ");
                    need = IS_NEDDED(prop_ptr->FLAGS) ? "YES" : "NO";
                    fprintf(stdout, "%s", need);

                    fprintf(stdout, "\n\t\tOptimizable: ");
                    optimizable = IS_OPTIMIZABLE(prop_ptr->FLAGS) ? "YES" : "NO";
                    fprintf(stdout, "%s", optimizable);

                    fprintf(stdout, "\n\t\tMIN: ");
                    fprintf(stdout, "%d", prop_ptr->iRANGE[0]);

                    fprintf(stdout, "\n\t\tMAX: ");
                    fprintf(stdout, "%d", prop_ptr->iRANGE[1]);

                    fprintf(stdout, "\n\t\tVALUE: ");
                    fprintf(stdout, "%d\n", prop_ptr->iINIT);
                    
                    break;
                case pDOUBLE:
                    fprintf(stdout, "\n\t\tType: DOUBLE");

                    fprintf(stdout, "\n\t\tMandatory: ");
                    need = IS_NEDDED(prop_ptr->FLAGS) ? "YES" : "NO";
                    fprintf(stdout, "%s", need);

                    fprintf(stdout, "\n\t\tOptimizable: ");
                    optimizable = IS_OPTIMIZABLE(prop_ptr->FLAGS) ? "YES" : "NO";
                    fprintf(stdout, "%s", optimizable);

                    fprintf(stdout, "\n\t\tMIN: ");
                    fprintf(stdout, "%f", prop_ptr->fRANGE[0]);

                    fprintf(stdout, "\n\t\tMAX: ");
                    fprintf(stdout, "%f", prop_ptr->fRANGE[1]);

                    fprintf(stdout, "\n\t\tVALUE: ");
                    fprintf(stdout, "%f\n", prop_ptr->fINIT);
                    break;
                case pSTR:
                    fprintf(stdout, "\n\t\tType: STR");

                    fprintf(stdout, "\n\t\tMandatory: ");
                    need = IS_NEDDED(prop_ptr->FLAGS) ? "YES" : "NO";
                    fprintf(stdout, "%s", need);

                    fprintf(stdout, "\n\t\tOptimizable: ");
                    optimizable = IS_OPTIMIZABLE(prop_ptr->FLAGS) ? "YES" : "NO";
                    fprintf(stdout, "%s", optimizable);

                    fprintf(stdout, "\n\t\tVALUES: ");
                    T_PSTR *ptr = prop_ptr->OPTS;
                    while (*ptr != NULL) {
                        fprintf(stdout, "%s\n\t\t\t", *ptr);
                        ptr++;
                    }
                    break;
                default:
                    fprintf(stdout, "Error: The configuration type has not yet been implemented\n");
            }
        }
    }
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

T_VOID listSelectedOutputOptions() {
    if (!OUTPUT_LIST_SELECTED) {
        fprintf(stdout, "Info: No selected outputs\n");
        return;
    }

    OUTPUT_LIST *iter = OUTPUT_LIST_SELECTED;
    size_t idx = 0;
    do {
        const T_PSTR graphTypeName = __getTypeName(OUTPUT_GRAPH_OPTIONS, NUM_OUTPUT_GRAPHS,iter->OUT->GRAPH_TYPE);
        const T_PSTR dataTypeName = __getTypeName(OUTPUT_DATA_OPTIONS, NUM_OUTPUT_DATA,iter->OUT->DATA_TYPE);
        if (!graphTypeName) return;
        if (!dataTypeName) return;
        fprintf(stdout, "[%ld]\t%s\tDATA: %s\tTYPE: %s\n", idx, iter->OUT->NAME, dataTypeName, graphTypeName);
        iter = iter->NEXT;
        idx++;
    } while (iter != NULL);
}



/************** ADDING/MODIFYING STATE FUNCTIONS ****************/
void loadAvailableArchs() {
    FILE *archs;

    if (!(archs = fopen(ARCHS_PATH ARCH_CONFIG, "r")))
        perror("Error: Couldn't open architecture configuration file");


    char buf[128];
    int i = 0;
    while (fgets(buf, sizeof(buf), archs)) {
        if (!isalnum(buf[0])) continue;
        if (buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = '\0';          // Get rid of the line feed
       
        memcpy(AVAIL_ARCHS.arch[i].name, buf, strlen(buf)+1);
        memcpy(AVAIL_ARCHS.arch[i].path, ARCHS_PATH, strlen(ARCHS_PATH)+1);
        strcat(AVAIL_ARCHS.arch[i].path, buf);
        strcat(AVAIL_ARCHS.arch[i].path, "/module/bin/dmodule.so");
        i++;
    }
    AVAIL_ARCHS.num = i;

    fclose(archs);
}

void loadAvailableConfigs() {
    FILE *configs;

    
    char config_list_path[256];

    memcpy(config_list_path, ARCHS_PATH, strlen(ARCHS_PATH)+1);
    strcat(config_list_path, SELECTED_ARCH.name);
    strcat(config_list_path, "/configs.list");


    if (!(configs = fopen(config_list_path, "r")))
        perror("Error: Couldn't open configuration list file");

    char buf[128];
    int i = 0;
    while (fgets(buf, sizeof(buf), configs)) {
        if (!isnotblank(buf[0])) continue;
        if (buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = '\0';          // Get rid of the line feed
       
        char *name = getNameFromPath(buf);
        memcpy(AVAIL_CONFIGS.config[i].name, name, strlen(name)+1);
        memcpy(AVAIL_CONFIGS.config[i].path, ARCHS_PATH, strlen(ARCHS_PATH)+1);
        strcat(AVAIL_CONFIGS.config[i].path, SELECTED_ARCH.name);
        strcat(AVAIL_CONFIGS.config[i].path, "/");
        strcat(AVAIL_CONFIGS.config[i].path, buf);
        i++;
    }
    AVAIL_CONFIGS.num = i;
    AVAIL_CONFIGS.selected = -1;

    fclose(configs);
}

T_VOID selectArch(size_t choice) {
    if (choice < 0 || 
        choice >= AVAIL_ARCHS.num) 
    {
        fprintf(stdout, "The selected arch %ld is not available", choice);
        return;
    }

    SELECTED_ARCH = AVAIL_ARCHS.arch[choice];
    fprintf(stdout, "%s was successfully selected\n", SELECTED_ARCH.name);

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

/*
 * This function does not correct the config file if additional proprieties/components are specified
 * This function calls BUILD_PROJECT of the module with a CONFIG argument that does not necessarily have
 * all the proprieties in the same order as specified by the module (some uneeded by be missing)
 */
T_VOID loadConfig(T_UINT config_option) {
    if (SELECTED_ARCH.name[0] == '\0') {
        fprintf(stderr, "Error: No config has been selected\n");
        return;
    }
    if (config_option < 0 || 
        config_option >= AVAIL_CONFIGS.num) 
    {
        fprintf(stderr, "Error: The selected option is not available\n");
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


T_ERROR addOutputOption(T_PSTR graph, T_PSTR data, T_PSTR name, size_t size, TYPE type) {
    T_UINT graph_type_idx = __getType(OUTPUT_GRAPH_OPTIONS, NUM_OUTPUT_GRAPHS, graph);
    T_UINT data_type_idx = __getType(OUTPUT_DATA_OPTIONS, NUM_OUTPUT_DATA, data);
    if (graph_type_idx == -1) {
        fprintf(stderr, "Error: The specified output type (%s) does not exist\n", graph);
        return -1;
    }
    if (data_type_idx == -1) {
        fprintf(stderr, "Error: The specified data type (%s) does not exist\n", data);
        return -1;
    }
    assert(name != NULL);
    if (!strlen(name)) {
        fprintf(stderr, "Error: No name was specified for the output\n");
        return -1;
    }

    OUTPUT *entry;
    if (!OUTPUT_LIST_SELECTED) {
        // The list is empty
        OUTPUT_LIST_SELECTED = (OUTPUT_LIST *) calloc(1, sizeof(OUTPUT_LIST));
        if (!OUTPUT_LIST_SELECTED)
            return -1;
        OUTPUT_LIST_SELECTED->OUT = (OUTPUT *) calloc(1, sizeof(OUTPUT));
        entry = OUTPUT_LIST_SELECTED->OUT;
        goto NEW_ENTRY;
    }

    OUTPUT_LIST *iter = OUTPUT_LIST_SELECTED, *prev = NULL;
    while (iter != NULL) {
        if (!strcmp(iter->OUT->NAME, name)) {
            entry = iter->OUT;
            goto UPDATE_ENTRY;
        }
        prev = iter;
        iter = iter->NEXT;
    }

    // If this point reaches it means there is not an entry w\ the same type
    OUTPUT_LIST *new_list_entry = (OUTPUT_LIST *) calloc(1, sizeof(OUTPUT_LIST));
    new_list_entry->NEXT = NULL;
    new_list_entry->OUT = (OUTPUT *) calloc(1, sizeof(OUTPUT));
    prev->NEXT = new_list_entry;
    entry = new_list_entry->OUT;

NEW_ENTRY:
    strcpy(entry->NAME, name);
UPDATE_ENTRY:
    entry->TYPE = type;
    entry->DATA_SIZE = size;
    if (graph != NULL)
        entry->GRAPH_TYPE = graph_type_idx;
    if (data != NULL)
        entry->DATA_TYPE = data_type_idx;
    return 0;
}

/************** DELETING STATE FUNCTIONS ****************/
T_ERROR deleteOutputOption(T_PSTR name) {
    OUTPUT_LIST *iter = OUTPUT_LIST_SELECTED;
    OUTPUT_LIST *prev = NULL;

    while (iter != NULL) {
        if (!strcmp(name, iter->OUT->NAME)) {
            if (prev) {
                prev->NEXT = iter->NEXT;
            } else {
                OUTPUT_LIST_SELECTED = iter->NEXT;
            }
            free(iter->OUT);
            free(iter);

            return 0;
        }

        prev = iter;
        iter = iter->NEXT;
    }
    return -1;
}

T_VOID destroyConfig(CONFIG *cfg) {
    if (cfg) {
        for (size_t comp_idx = 0; comp_idx < cfg->NUM; comp_idx++) {
            COMP *comp = *(cfg->COMPS + comp_idx);

            free(comp->PBUFFER->PROPS);
            free(comp->PBUFFER);
            free(comp);
        }
        free(cfg);
    }
}

T_ERROR cleanState() {
    // Clean INPUT_CONFIG
    destroyConfig(INPUT_CONFIG);
    INPUT_CONFIG = NULL;

    // Clean OUTPUT_LIST_SELECTED
    OUTPUT_LIST *iter = OUTPUT_LIST_SELECTED;
    while (iter != NULL) {
        deleteOutputOption(iter->OUT->NAME);
        iter = OUTPUT_LIST_SELECTED;
    }

    if (MODULE_HANDLE) {
        dlclose(MODULE_HANDLE);
        MODULE_HANDLE = NULL;
    }

    return 0;
}

/************** HELPER STATE FUNCTIONS ****************/
static T_UINT __getType(T_STR *type_options, size_t num_options, T_PSTR type) {
    if (type == NULL) return 0;
    for (size_t type_idx = 0 ; type_idx < num_options ; type_idx++ ) {
        if (!strcmp(type_options[type_idx], type)) {
            return type_idx;
        }
    }
    return -1;
}

static const T_PSTR __getTypeName(T_STR *type_options, size_t num_options, T_UINT type) {
    if (type >= num_options) {
        fprintf(stderr, "Error: The type of an output (%d) is not well defined\n", type);
        return NULL;
    }
    return type_options[type];
}

static CONFIG *parseConfig(FILE *fd) {
    CONFIG *config = calloc(sizeof(CONFIG), 1);

    char buffer[256];
    size_t line_num = 0;

    COMP *component = NULL;
    pBUFFER *prop_buffer = NULL;
    while (fgets(buffer, sizeof(buffer), fd)) {
        line_num++;
        char *init_buffer_ptr = buffer;
        char *end_buffer_ptr = buffer;

        GET_FIRST_CHAR(init_buffer_ptr);
        if (*init_buffer_ptr == '\n') continue;          // Ignore empty line
        // If the character '#' is detected it initiates a new component with the corresponding ID
        if (*init_buffer_ptr == '#') {
            // Create a new component
            component = calloc(sizeof(COMP), 1);
            component->ID = parseNum(init_buffer_ptr + 1);
            // Create Propriety Buffer and link it to the newly created component
            prop_buffer = calloc(sizeof(pBUFFER), 1);
            component->PBUFFER = prop_buffer;
            // Link an entry of the config components to the newly created component
            config->COMPS[config->NUM] = component;
            config->NUM++;

            fprintf(stdout, "Info: New Component ID: %d\n", component->ID);
            continue;
        }

        GET_FIRST_OCUR(end_buffer_ptr, ':');
        // In case no ':' is found, the propriety is badly formed
        // In case the init ptr and end ptr are the same (no propriety name) throw an error
        if (*end_buffer_ptr == '\n' || \
            init_buffer_ptr == end_buffer_ptr) 
        {
            fprintf(stderr, "Error: Parser didn't manage to get propriety value in line %ld\n", line_num);
            return NULL;
        }

        prop_buffer->NUM++;
        prop_buffer->PROPS = realloc(prop_buffer->PROPS, sizeof(PROP)*prop_buffer->NUM);
        PROP *new_prop = prop_buffer->PROPS + prop_buffer->NUM -1;
        memcpy(new_prop->NAME, init_buffer_ptr, end_buffer_ptr-init_buffer_ptr);
        new_prop->NAME[end_buffer_ptr-init_buffer_ptr] = '\0';
        fprintf(stdout, "Info: Propriety %s\t", new_prop->NAME);

        end_buffer_ptr++;
        init_buffer_ptr = end_buffer_ptr;       
        GET_FIRST_CHAR(init_buffer_ptr);        // Locate the first character for value
        end_buffer_ptr = init_buffer_ptr;
        GET_FIRST_OCUR(end_buffer_ptr, ' ');    // Make end ptr point towards either end of input or next space

        if (isdigit(*init_buffer_ptr)) {
            if (*(end_buffer_ptr -1) == 'f') {
                new_prop->fINIT = parseFloat(init_buffer_ptr);
                new_prop->PTYPE = pDOUBLE;
                fprintf(stdout, "type %s\t", "DOUBLE");
                fprintf(stdout, "value %f\n", new_prop->fINIT);
            } else {
                new_prop->iINIT = parseNum(init_buffer_ptr);
                new_prop->PTYPE = pINT;
                fprintf(stdout, "type %s\t", "INT");
                fprintf(stdout, "value %d\n", new_prop->iINIT);
            }
        }
        else if (isalpha(*init_buffer_ptr)) {
            new_prop->PTYPE = pSTR;
            memcpy(new_prop->sINIT, init_buffer_ptr, end_buffer_ptr-init_buffer_ptr);
            new_prop->sINIT[end_buffer_ptr-init_buffer_ptr] = '\0';
            fprintf(stdout, "type %s\t", "STR");
            fprintf(stdout, "value %s\n", new_prop->sINIT);
        } else {
            fprintf(stderr, "Error: Parser didn't manage to get propriety value in line %ld\n", line_num);
            return NULL;
        }

        // Extension for FLAGS
        init_buffer_ptr = end_buffer_ptr;
        GET_FIRST_OCUR(init_buffer_ptr, '[');
        GET_FIRST_OCUR(end_buffer_ptr, ']');
        
        new_prop->FLAGS = 0;
        while (init_buffer_ptr < end_buffer_ptr) {
            GET_FIRST_CHAR(init_buffer_ptr);
            switch (*init_buffer_ptr) {
                case 'O':
                    fprintf(stdout, "Info: Propriety %s is optimizable\n", new_prop->NAME);
                    new_prop->FLAGS |= OPTIMIZABLE;
                    break;
                case 'M':
                    fprintf(stdout, "Info: Propriety %s is a mitigation\n", new_prop->NAME);
                    new_prop->FLAGS |= MITIGATION;
                    break;
                case 'N':
                    fprintf(stdout, "Info: Propriety %s is needed\n", new_prop->NAME);
                    new_prop->FLAGS |= NEEDED;
                    break;
                default:
                    fprintf(stdout, "Info: Propriety %s has an invalid attribute\n", new_prop->NAME);
            }
            init_buffer_ptr++;
            GET_FIRST_CHAR(init_buffer_ptr);
        }
    }

    if (GET_COMP_BY_IDX(config, MODULE_CONFIG->VICTIM_ID, NULL) == -1) {
        fprintf(stderr, "Error: The victim component (ID %ld) was not configured", MODULE_CONFIG->VICTIM_ID);
        return NULL;
    }

    return config;
}
