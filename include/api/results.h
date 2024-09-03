#ifndef RESULTS_H
#define RESULTS_H

#include "types.h"

/************** TYPES DEFINITION ****************/
typedef struct {
    T_STR       NAME;
    G_ARRAY     ARRAY;
} RESULT;


/************** MACRO DEFINITION ****************/
#define INITIALIZE_RESULTS(T, p_results, num_cycles, name) \
    __##T##_initializeResults(p_results, num_cycles, name);

#define REGISTER_OUTPUT(T, size, name) \
    __##T##_registerOutput(size, name)

/************** RESULT INITIALIZATION ****************/
void __T_UINT_initializeResults(RESULT *results_ptr, T_UINT num_cycles, const T_PSTR name);
void __T_DOUBLE_initializeResults(RESULT *results_ptr, T_UINT num_cycles, const T_PSTR name);

/************** RESULT DESTRUCTION ****************/
T_VOID DESTROY_RESULTS(RESULT *result_ptr);


#endif
