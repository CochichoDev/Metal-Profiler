#pragma once

#include <stddef.h>
#include <stdint.h>

#define BASEDIR "arch/"

typedef char FLAG;

typedef int32_t INT;
typedef char STR[64];
typedef double DOUBLE;
typedef char *STR_P;

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
