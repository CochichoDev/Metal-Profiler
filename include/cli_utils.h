#pragma once

#include "api/api.h"
#include "cli.h"
#include <cstddef>

uint8_t listArchs(TERM *term);
void printConfig(TERM *term);
void selectArch(TERM *term, size_t choice);
void loadConfig(T_STR config_path);
T_VOID executeBench(TERM *term, size_t iter);
