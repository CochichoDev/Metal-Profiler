#ifndef GENERICS_H
#define GENERICS_H

#include "types.h"

/************** TYPES DEFINITION ****************/

/************** MACRO DEFINITION ****************/
#define INIT_GENERIC(T, P_DATA, SIZE)       \
    __##T##__InitializeGeneric(P_DATA, SIZE);


/************** GENERIC INITIALIZATION ****************/
T_VOID __T_INT__InitializeGeneric(G_ARRAY* gen_data, size_t size);
T_VOID __T_UINT__InitializeGeneric(G_ARRAY* gen_data, size_t size);
T_VOID __T_DOUBLE__InitializeGeneric(G_ARRAY* gen_data, size_t size);

/************** GENERIC DESTRUCTION ****************/
T_VOID DESTROY_GENERIC(G_ARRAY* gen_data);

#endif
