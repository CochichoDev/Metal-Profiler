#pragma once

#include <stddef.h>
#include <stdint.h>

#include <sys/types.h>



#define BASEDIR "arch/"

typedef char FLAG;

typedef int32_t     T_INT;
typedef uint32_t    T_UINT;
typedef char        T_CHAR;
typedef double      T_DOUBLE;
typedef char        T_STR[64];
typedef char        *T_PSTR;
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
    
    FLAG NEED;

    // If the type is an int
    union {
        T_INT iRANGE[2];
        T_DOUBLE fRANGE[2];
    };

    union {
        T_INT iINIT;
        T_DOUBLE fINIT;
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

/*
 * Generic type of Results
 */
typedef struct {
    T_STR       NAME;
    size_t      NUM;
    T_VOID      *DATA;
    enum {
        R_INT,
        R_UINT,
        R_DOUBLE
    }           TYPE;
} RESULT;


#define INITIALIZE_RESULTS(T, p_results, num_cycles, name) \
    __##T##_initializeResults(p_results, num_cycles, name);

#define DESTROY_RESULTS(T, results) \
    __##T##_destroyResults(results);

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
 *      out : Returns the pointer to the corresponding component if the pointer is not NULL
 *      default : Returns the index from the base pointer of the COMPS pointer of the CONFIG or -1 if it doesn't exist
 */
size_t GET_COMP_BY_IDX(CONFIG *in1, T_INT in2, COMP **out);

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
size_t GET_PROP_BY_NAME(COMP *in1, T_PSTR in2, T_VOID *out);


pid_t RUN_PROCESS_IMAGE(T_INT *new_descr, const T_PSTR image_path, ...);
void KILL_PROCESS(pid_t process);
