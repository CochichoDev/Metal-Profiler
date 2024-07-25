#pragma once

#include "api/api.h"
#include "cli.h"

uint8_t listArchs(TERM *term);
uint8_t listConfigs(TERM *term);

void printConfig(TERM *term);
T_VOID listOutputTypes();
void selectArch(TERM *term, size_t choice);
void loadConfig(TERM *term, T_UINT config_option);
T_VOID executeBench(TERM *term, size_t iter);

