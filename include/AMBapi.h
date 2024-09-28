#ifndef AMB_H
#define AMB_H

/*****************INCLUDE FILES*****************/
#include <dlfcn.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/types.h>

#define TTY_PORT    "/dev/ttyUSB0"

#define ARCHS_PATH "arch/"
#define ARCH_CONFIG "archs.txt"

#define T32SCRIPT "launch_bench.cmm"
#define XSCTSCRIPT "launch_bench.tcl"

#define T32EXECUTABLE "/opt/t32/bin/pc_linux64/t32marm64-qt"
#define XSCTEXECUTABLE "/tools/Xilinx/Vitis/2021.2/bin/xsct"

#define FALSE   0
#define TRUE    1

#define NEEDED          0b00000001
#define OPTIMIZABLE     0b00000010
#define MITIGATION      0b00000100

/*****************MACRO DEFINITION*****************/
#define IS_MITIGATION(X)   \
                (((X) >> 2) & 1U)

#define IS_OPTIMIZABLE(X)   \
                (((X) >> 1) & 1U)

#define IS_NEDDED(X)   \
                (((X) >> 0) & 1U)

/*****************TYPE DEFINITION*****************/
typedef int8_t      AMB_FLAG;
typedef int32_t     AMB_INT;
typedef uint32_t    AMB_UINT;
typedef uint16_t    AMB_USHORT;
typedef char        AMB_CHAR;
typedef uint8_t     AMB_UCHAR;
typedef double      AMB_DOUBLE;
typedef char        AMB_STR[64];
typedef char       *AMB_PSTR;
typedef void        AMB_VOID;
typedef int8_t      AMB_ERROR;

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
    AMB_STR NAME;

    pTYPE PTYPE;
    
    AMB_FLAG FLAGS;

    // If the type is an int
    union {
        AMB_INT iRANGE[2];
        AMB_DOUBLE fRANGE[2];
    };

    union {
        AMB_INT iINIT;
        AMB_DOUBLE fINIT;
    };

    union {
        AMB_INT iSTEP;
        AMB_DOUBLE fSTEP;
    };
        
    AMB_PSTR OPTS[32];
    AMB_STR sINIT;
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
    AMB_INT ID;
    AMB_STR NAME;
    pBUFFER *PBUFFER;
} COMP;

typedef struct {
    COMP        *COMPS[16];
    size_t      VICTIM_ID;
    size_t      NUM;
} CONFIG;

typedef struct {
    T_STR       NAME;
    G_ARRAY     ARRAY;
} RESULT;

#define REGISTER_OUTPUT(T, size, name) \
    __##T##_registerOutput(size, name)

/*****************FUNCTION DECLARATION*****************/
/*
 * GET_COMP_BY_ID : Finds the component that the input index corresponds to
 * PARAMETERS:
 *      in1 : The associated config
 *      in2 : The ID of the component to be searched for
 * RETURN:
 *      out : Returns the pointer to the corresponding component if the pointer is not NULL
 *      default : Returns the index from the base pointer of the COMPS pointer of the CONFIG or -1 if it doesn't exist
 */
size_t GET_COMP_BY_ID(CONFIG *in1, AMB_INT in2, const COMP **out);

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
size_t GET_PROP_BY_NAME(const COMP *const in1, AMB_PSTR in2, AMB_VOID *out);

pid_t RUN_PROCESS_IMAGE(AMB_INT *new_descr, const AMB_PSTR image_path, ...);
void KILL_PROCESS(pid_t process);

AMB_ERROR CALL_MAKEFILES(CONFIG *config);

/*
 * #############################
 * INTERNAL FUNCTION DECLARATION
 * #############################
 */

AMB_VOID __T_UINT_registerOutput(size_t size, char *name);
AMB_VOID __T_DOUBLE_registerOutput(size_t size, char *name);

AMB_VOID UNREGISTER_OUTPUT(RESULT *results);


/*****************TTY FUNCTION DECLARATION*****************/
AMB_ERROR INIT_TTY(const char *path);
AMB_VOID CLOSE_TTY();
void TTY_TO_RESULT(AMB_CHAR imarker, AMB_CHAR fmarker, RESULT *results);

/*****************XSCT FUNCTION DECLARATION*****************/
AMB_INT INIT_XSCT(char *scriptname);
AMB_INT EX_XSCT_SCRIPT(const char *scriptname, size_t num_cores, AMB_FLAG core_state[]);
AMB_VOID CLOSE_XSCT();

#endif
