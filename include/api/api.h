#pragma once

#include <stddef.h>
#include <stdint.h>

#define BASEDIR "arch/"

typedef char FLAG;

typedef int32_t INT;
typedef char CHAR;
typedef double DOUBLE;
typedef char STR[64];
typedef char *STR_P;
typedef void VOID;

/*
 * ###############################
 * TYPE DEFINITION
 * ###############################
 */
/*
 * Propriety type
 */
typedef enum {
    pINT,
    pDOUBLE,
    pCHAR,
    pSTR
} pTYPE;

/*
 * Propriety structure
 */
typedef struct {
    STR NAME;

    pTYPE PTYPE;
    
    FLAG NEED;

    // If the type is an int
    union {
        INT iRANGE[2];
        DOUBLE fRANGE[2];
    };

    union {
        INT iINIT;
        DOUBLE fINIT;
    };
        
    STR_P OPTS[32];
    STR sINIT;
} PROP;


/*
 * Buffer strucutre holding multiple proprieties
 */
typedef struct {
    PROP *PROPS;
    size_t NUM;
} pBUFFER;

/*
 * Component structure
 * Holds an ID, NAME and multiple proprieties
 */
typedef struct {
    INT ID;
    STR NAME;
    pBUFFER *PBUFFER;
} COMP;

typedef struct {
    COMP *COMPS[16];
    size_t NUM;
} CONFIG;

/*
 * ###############################
 * FUNCTION DECLARATION
 * ###############################
 */
/*
 * GET_COMP_BY_IDX : Finds the component that the input index corresponds to
 * PARAMETERS:
 *      in1 : The associated config
 *      in2 : The index of the component to be searched for
 * RETURN:
 *      out : Returns the pointer to the corresponding component
 *      default : Returns the index from the base pointer of the COMPS pointer of the CONFIG
 */
size_t GET_COMP_BY_IDX(CONFIG *in1, INT in2, COMP **out);

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
size_t GET_PROP_BY_NAME(COMP *in1, STR_P in2, VOID *out);
