#ifndef XSCT_I_H
#define XSCT_I_H

#include "types.h"

T_INT INIT_XSCT(char *scriptname);
T_INT EX_XSCT_SCRIPT(const char *scriptname, size_t num_cores, T_FLAG core_state[]);
T_VOID CLOSE_XSCT();

#endif
