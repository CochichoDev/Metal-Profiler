#pragma once

#include "api/api.h"
#include "cli.h"

T_ERROR listArchs(TERM *term);
T_ERROR listConfigs(TERM *term);

T_VOID printConfig(TERM *term);
T_VOID listOutputTypes();
T_VOID selectArch(TERM *term, size_t choice);
T_VOID loadConfig(TERM *term, T_UINT config_option);
T_VOID runExecution (size_t iter);
T_ERROR listState();

