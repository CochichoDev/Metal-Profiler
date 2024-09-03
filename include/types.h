#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>

typedef int8_t      T_FLAG;

typedef int32_t     T_INT;
typedef uint32_t    T_UINT;
typedef uint16_t    T_USHORT;
typedef char        T_CHAR;
typedef uint8_t     T_UCHAR;
typedef double      T_DOUBLE;
typedef char        T_STR[64];
typedef char       *T_PSTR;
typedef void        T_VOID;
typedef int8_t      T_ERROR;

/*
 * ###############################
 * TYPE DEFINITION
 * ###############################
 */
/*
 * Propriety type
 */
typedef enum {
    pBOOL,
    pINT,
    pDOUBLE,
    pCHAR,
    pSTR
} pTYPE;

/*
 * Propriety structure
 */
typedef struct {
    T_STR NAME;

    pTYPE PTYPE;
    
    T_FLAG FLAGS;

    // If the type is an int
    union {
        T_INT iRANGE[2];
        T_DOUBLE fRANGE[2];
    };

    union {
        T_INT iINIT;
        T_DOUBLE fINIT;
    };

    union {
        T_INT iSTEP;
        T_DOUBLE fSTEP;
    };
        
    T_PSTR OPTS[32];
    T_STR sINIT;
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
    T_INT ID;
    T_STR NAME;
    pBUFFER *PBUFFER;
} COMP;

typedef struct {
    COMP        *COMPS[16];
    size_t      VICTIM_ID;
    size_t      NUM;
} CONFIG;

typedef enum {
        G_INT,
        G_UINT,
        G_DOUBLE,
        G_RESULT, 
        G_METRICS
} TYPE;

typedef struct {
    T_VOID      *DATA;
    size_t      SIZE;
    TYPE        TYPE;
} G_ARRAY;
#endif
