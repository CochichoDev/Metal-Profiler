#include <string.h>

#include "api/api.h"

/*
 * GET_COMP_BY_IDX : Finds the component that the input index corresponds to
 * PARAMETERS:
 *      in1 : The associated config
 *      in2 : The index of the component to be searched for
 * RETURN:
 *      out : Returns the pointer to the corresponding component
 *      default : Returns the index from the base pointer of the COMPS pointer of the CONFIG
 */
size_t GET_COMP_BY_IDX(CONFIG *in1, INT in2, COMP **out) {
    for (size_t idx = 0 ; idx < in1->NUM ; idx++) {
        if (in1->COMPS[idx]->ID == in2) {
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
 *      out : Returns the value the corresponding propriety
 *      default : Returns the index from the base pointer of the PROPS pointer of the PBUFFER
 *                  associated PBUFFER of the component inputed
 */
size_t GET_PROP_BY_NAME(COMP *in1, STR_P in2, VOID *out) {
    for (size_t idx = 0 ; idx < in1->PBUFFER->NUM ; idx++) {
        if (!strcmp(in1->PBUFFER->PROPS[idx].NAME, in2)) {
            switch (in1->PBUFFER->PROPS[idx].PTYPE) {
                case pINT:
                    *(INT *) out = in1->PBUFFER->PROPS[idx].iINIT;
                    break;
                case pDOUBLE:
                    *(DOUBLE *) out = in1->PBUFFER->PROPS[idx].fINIT;
                    break;
                case pSTR:
                    *(STR_P *) out = in1->PBUFFER->PROPS[idx].sINIT;
                    break;
                case pCHAR:
                    *(CHAR *) out = in1->PBUFFER->PROPS[idx].iINIT;
                    break;
            }
            return idx;
        }
    }
    return -1;
}
