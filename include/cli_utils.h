#pragma once

#include "api/api.h"
#include "cli.h"

uint8_t listArchs(TERM *term);
void printConfig(TERM *term);
void selectArch(TERM *term, size_t choice);
void loadConfig(T_STR config_path);
void executeBench(TERM *term);
