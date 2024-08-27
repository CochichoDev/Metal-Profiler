#pragma once

#include <dlfcn.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/types.h>

#define BASEDIR "arch/"

#define FALSE   0
#define TRUE    1

#define NEEDED          0b00000001
#define OPTIMIZABLE     0b00000010
#define MITIGATION      0b00000100

#define IS_MITIGATION(X)   \
                (((X) >> 2) & 1U)

#define IS_OPTIMIZABLE(X)   \
                (((X) >> 1) & 1U)

#define IS_NEDDED(X)   \
                (((X) >> 0) & 1U)

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

/*
 * Generic type of Results
 */
typedef struct {
    T_STR       NAME;
    G_ARRAY     ARRAY;
} RESULT;


#define INITIALIZE_RESULTS(T, p_results, num_cycles, name) \
    __##T##_initializeResults(p_results, num_cycles, name);

#define REGISTER_OUTPUT(T, size, name) \
    __##T##_registerOutput(size, name)

/*
 * ###############################
 * FUNCTION DECLARATION
 * ###############################
 */
T_VOID DESTROY_RESULTS(RESULT *result_ptr);
/*
 * GET_COMP_BY_IDX : Finds the component that the input index corresponds to
 * PARAMETERS:
 *      in1 : The associated config
 *      in2 : The ID of the component to be searched for
 * RETURN:
 *      out : Returns the pointer to the corresponding component if the pointer is not NULL
 *      default : Returns the index from the base pointer of the COMPS pointer of the CONFIG or -1 if it doesn't exist
 */
size_t GET_COMP_BY_IDX(CONFIG *in1, T_INT in2, const COMP **out);

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
size_t GET_PROP_BY_NAME(const COMP *const in1, T_PSTR in2, T_VOID *out);


pid_t RUN_PROCESS_IMAGE(T_INT *new_descr, const T_PSTR image_path, ...);
void KILL_PROCESS(pid_t process);

/*
 * #############################
 * INTERNAL FUNCTION DECLARATION
 * #############################
 */
void __T_UINT_initializeResults(RESULT *results_ptr, T_UINT num_cycles, const T_PSTR name);
void __T_DOUBLE_initializeResults(RESULT *results_ptr, T_UINT num_cycles, const T_PSTR name);
void __T_UINT_destroyResults(RESULT *results_ptr);
void __T_DOUBLE_destroyResults(RESULT *results_ptr);

T_VOID __T_UINT_registerOutput(size_t size, char *name);
T_VOID __T_DOUBLE_registerOutput(size_t size, char *name);

T_VOID UNREGISTER_OUTPUT(RESULT *results);

T_VOID READ_TO_RESULT(T_INT in, T_CHAR marker, RESULT *results);
