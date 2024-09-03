#ifndef API_H
#define API_H

#include <dlfcn.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/types.h>

#include "types.h"
#include "results.h"

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

/*
 * GET_COMP_BY_ID : Finds the component that the input index corresponds to
 * PARAMETERS:
 *      in1 : The associated config
 *      in2 : The ID of the component to be searched for
 * RETURN:
 *      out : Returns the pointer to the corresponding component if the pointer is not NULL
 *      default : Returns the index from the base pointer of the COMPS pointer of the CONFIG or -1 if it doesn't exist
 */
size_t GET_COMP_BY_ID(CONFIG *in1, T_INT in2, const COMP **out);

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

T_VOID __T_UINT_registerOutput(size_t size, char *name);
T_VOID __T_DOUBLE_registerOutput(size_t size, char *name);

T_VOID UNREGISTER_OUTPUT(RESULT *results);

#endif
