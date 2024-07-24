#pragma once

#include "api.h"

pid_t INIT_T32(const T_PSTR path_to_exec);
T_INT CLOSE_T32(pid_t t32_pid);
T_INT INIT_T32_CONN(const char *node, const char *port);
T_INT CLOSE_T32_CONN();
T_INT EX_T32_SCRIPT(const char *scriptname);
