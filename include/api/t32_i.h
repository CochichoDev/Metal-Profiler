#ifndef T32_I_H
#define T32_I_H

#include "api.h"

T_ERROR INIT_T32(const T_PSTR path_to_exec);
T_INT CLOSE_T32();
T_INT INIT_T32_CONN(const char *node, const char *port);
T_INT CLOSE_T32_CONN();
T_INT EX_T32_SCRIPT(const char *scriptname, size_t num_cores, T_FLAG core_state[]);

#endif
