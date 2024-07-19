#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "cli.h"
#include "cli_utils.h"
#include "global.h"

uint8_t listArchs(TERM *term) {
    char buf[256];
    for (size_t i = 0; i < AVAIL_ARCHS.num ; i++) {
        sprintf(buf, "[%ld]\t%s\n", i, AVAIL_ARCHS.arch[i].name); 
        write(term->out_descr, buf, strlen(buf));
    }
    
    return 0;
}

#define ERROR_ARCH "Error: No architecture has been selected\n"
#define MSG_INT0 "\n\t\tType: INT"
#define MSG_STR0 "\n\t\tType: STR"
#define MSG1 "\n\t\tMandatory: "
#define MSG_INT2 "\n\t\tMIN: "
#define MSG_INT3 "\n\t\tMAX: "
#define MSG_INT4 "\n\t\tVALUE: "
#define MSG_STR2 "\n\t\tVALUES: "
#define ERROR_TYPE_NOT_IMPLEMENTED "Error: The configuration type has \
                                    not yet been implemented\n"

static uint8_t getTypeSize(pTYPE t) {
    switch (t) {
        case pINT:
            return sizeof(INT);
        case pSTR:
            return sizeof(STR);
        case pDOUBLE:
            return sizeof(DOUBLE);
        default:
            return 0;
    }
}

void printConfig(TERM *term) {
    if (SELECTED_ARCH.name[0] == '\0') {
        write(term->out_descr, ERROR_ARCH, sizeof(ERROR_ARCH));
        return;
    }

    COMP **comp_ptr = MODULE_CONFIG->COMPS;
    for (size_t cidx = 0 ; cidx < MODULE_CONFIG->NUM ; cidx++, comp_ptr++) {
        // Name of the component
        write(term->out_descr, (*comp_ptr)->NAME, strlen((*comp_ptr)->NAME));

        PROP *prop_ptr = (*comp_ptr)->PBUFFER->PROPS;
        for (size_t pidx = 0 ; pidx < (*comp_ptr)->PBUFFER->NUM ; pidx++, prop_ptr++) {
            write(term->out_descr, "\n\t", 2);
            write(term->out_descr, prop_ptr->NAME, strlen(prop_ptr->NAME));
            FLAG *need;
            char num[16];
            switch (prop_ptr->PTYPE) {
                case pINT:
                    write(term->out_descr, MSG_INT0, sizeof(MSG_INT0));

                    write(term->out_descr, MSG1, sizeof(MSG1));

                    need = prop_ptr->NEED ? "YES" : "NO";
                    write(term->out_descr, need, strlen(need));

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

                    need = prop_ptr->NEED ? "YES" : "NO";
                    write(term->out_descr, need, strlen(need));

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

                    need = prop_ptr->NEED ? "YES" : "NO";
                    write(term->out_descr, need, strlen(need));

                    write(term->out_descr, MSG_STR2, sizeof(MSG_STR2));
                    STR_P *ptr = prop_ptr->OPTS;
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
void selectArch(TERM *term, size_t choice) {
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
        perror("Error: Could not open handle of module");
    if (!(MODULE_CONFIG = (CONFIG *) dlsym(MODULE_HANDLE, "ARCH_CONFIG")))
        perror("Error: Could not access CONFIG variable");

    if (!(LOAD_CONFIG = (void (*)(void**)) dlsym(MODULE_HANDLE, "callMakefiles")))
        perror("Error: Could not access callMakefiles function");
}

void loadConfig() {
    /*
    void **config = malloc(sizeof(void*) * MODULE_CONFIG->num);
    // Make sure that the memory that config points to is 0 initialized
    memset(config, 0, sizeof(void*) * MODULE_CONFIG->num);

    for (size_t i = 0 ; i < MODULE_CONFIG->num ; i++)
        *(config+i) = malloc(getTypeSize(MODULE_CONFIG->configptr[i].type));

    memcpy(*config, "Core0", 6);
    memcpy(*(config+4), "Core1", 6);
    memcpy(*(config+7), "Core2", 6);
    memcpy(*(config+10), "Core3", 6);

    LOAD_CONFIG(config);
    */
}
