#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "api.h"
#include "global.h"

static T_UINT __getType(T_STR *type_options, size_t num_options, T_PSTR type) {
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

T_ERROR addOutputOption(T_PSTR graph, T_PSTR data, T_PSTR name) {
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
    if (!strlen(name)) {
        fprintf(stderr, "Error: No name was specified for the output\n");
        return -1;
    }

    OUTPUT *entry;
    if (!OUTS.OUT) {
        // The list is empty
        OUTS.OUT = (OUTPUT *) malloc(sizeof(OUTPUT));
        entry = OUTS.OUT;
        goto NEW_ENTRY;
    }

    OUTPUT_LIST *iter = &OUTS, *prev;
    while (iter != NULL) {
        if (!strcmp(iter->OUT->NAME, name)) {
            entry = iter->OUT;
            goto UPDATE_ENTRY;
        }
        prev = iter;
        iter = iter->NEXT;
    }

    // If this point reaches it means there is not an entry w\ the same type
    OUTPUT_LIST *new_list_entry = (OUTPUT_LIST *) malloc(sizeof(OUTPUT_LIST));
    new_list_entry->NEXT = NULL;
    new_list_entry->OUT = (OUTPUT *) malloc(sizeof(OUTPUT_LIST));
    prev->NEXT = new_list_entry;
    entry = new_list_entry->OUT;

NEW_ENTRY:
    strcpy(entry->NAME, name);
UPDATE_ENTRY:
    entry->GRAPH_TYPE = graph_type_idx;
    entry->DATA_TYPE = data_type_idx;
    return 0;
}

T_VOID listSelectedOutputOptions() {
    if (!OUTS.OUT) {
        fprintf(stdout, "Info: No selected outputs\n");
        return;
    }

    OUTPUT_LIST *iter = &OUTS;
    size_t idx = 0;
    do {
        const T_PSTR graphTypeName = __getTypeName(OUTPUT_GRAPH_OPTIONS, NUM_OUTPUT_GRAPHS,iter->OUT->GRAPH_TYPE);
        const T_PSTR dataTypeName = __getTypeName(OUTPUT_DATA_OPTIONS, NUM_OUTPUT_DATA,iter->OUT->DATA_TYPE);
        if (!graphTypeName) return;
        if (!dataTypeName) return;
        fprintf(stdout, "[%ld]\t%s\tDATA: %s\tTYPE: %s\n", idx, iter->OUT->NAME, dataTypeName, graphTypeName);
        iter = iter->NEXT;
    } while (iter != NULL);
}
