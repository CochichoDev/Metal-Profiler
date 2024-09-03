/*
 * File: api.c
 * Functionality provided by the API to the modules
 * Author: Diogo Cochicho
 */

#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <fcntl.h>

#include "api.h"
#include "state.h"

/************** CONFIG MANIPULATION ****************/
/*
 * GET_COMP_BY_ID : Finds the component that the input index corresponds to
 * PARAMETERS:
 *      in1 : The associated config
 *      in2 : The ID of the component to be searched for
 * RETURN:
 *      out : Returns the pointer to the corresponding component if the pointer is not NULL
 *      default : Returns the index from the base pointer of the COMPS pointer of the CONFIG or -1 if it doesn't exist
 */
size_t GET_COMP_BY_ID(CONFIG *in1, T_INT in2, const COMP **out) {
    for (size_t idx = 0 ; idx < in1->NUM ; idx++) {
        if (in1->COMPS[idx]->ID == in2) {
            if (out != NULL)
                *out = in1->COMPS[idx];
            return idx;
        }
    }
    return -1;
}

/*
 * GET_PROP_BY_NAME : Finds the propriety that the input name corresponds to
 * PARAMETERS:
 *      in1 : The associated component
 *      in2 : The name of the propriety to be searched for
 * RETURN:
 *      out : Returns the value the corresponding propriety if the pointer passed is not NULL
 *      default : Returns the index from the base pointer of the PROPS pointer of the PBUFFER
 *                  associated PBUFFER of the component inputed or -1 if not found
 */
size_t GET_PROP_BY_NAME(const COMP *const in1, T_PSTR in2, T_VOID *out) {
    for (size_t idx = 0 ; idx < in1->PBUFFER->NUM ; idx++) {
        if (!strcmp(in1->PBUFFER->PROPS[idx].NAME, in2)) {
            if (out != NULL)
                switch (in1->PBUFFER->PROPS[idx].PTYPE) {
                    case pINT:
                        *(T_INT *) out = in1->PBUFFER->PROPS[idx].iINIT;
                        break;
                    case pDOUBLE:
                        *(T_DOUBLE *) out = in1->PBUFFER->PROPS[idx].fINIT;
                        break;
                    case pSTR:
                        *(T_PSTR *) out = in1->PBUFFER->PROPS[idx].sINIT;
                        break;
                    case pCHAR:
                        *(T_CHAR *) out = in1->PBUFFER->PROPS[idx].iINIT;
                        break;
                }
            return idx;
        }
    }
    return -1;
}


/************** PROCESS MANAGEMENT ****************/
pid_t RUN_PROCESS_IMAGE(T_INT *new_descr, const T_PSTR image_path, ...) {
    va_list va;
    const char *args[16];

    va_start(va, image_path);
    uint8_t idx;
    for (idx = 0; idx < sizeof(args)/sizeof(char*); idx++) {
        const char *arg = va_arg(va, char*);
        if (!arg) break;
        args[idx] = arg;
    }
    // The execv function needs to be ended with a NULL pointer
    args[idx] = NULL;
    va_end(va);


    pid_t child_process = fork();

    /*
     * In case the current process is the child
     * change the process image to the one demanded
     */
    if (child_process == -1) { 
        perror("Error: Could not fork the current process");
        return -1;
    }
    if (!child_process) {
        if (new_descr) {
            close(STDIN_FILENO);
            dup(new_descr[0]);
            close(STDOUT_FILENO);
            dup(new_descr[1]);
            close(STDERR_FILENO);
            dup(new_descr[2]);
        }
       
        if(execv(image_path, (char**) args) == -1) {
            perror("Error: Could not open the specified process image");
            exit(-1);
        }
    }

    return child_process;
}

void KILL_PROCESS(pid_t process) {
    kill(process, SIGTERM);
    if (kill<0) {
        perror("Error: Could not close child process");
        exit(1);
    }
}

/************** OUTPUT CONTROL ****************/
T_VOID __T_UINT_registerOutput(size_t size, char *name) {
    addOutputOption(NULL, NULL, name, size, G_UINT);
}

T_VOID __T_DOUBLE_registerOutput(size_t size, char *name) {
    addOutputOption(NULL, NULL, name, size, G_DOUBLE);
}

T_VOID UNREGISTER_OUTPUT(RESULT *results) {
    deleteOutputOption(results->NAME);
}
