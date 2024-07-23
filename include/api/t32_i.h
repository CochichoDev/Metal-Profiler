#pragma once

#include "api.h"

T_INT INIT_T32_CONN(const char *node, const char *port);
T_INT CLOSE_T32_CONN();
T_INT EX_T32_SCRIPT(const char *scriptname);
