#pragma once

#include "cli.h"

uint8_t listArchs(TERM *term);
void printConfig(TERM *term);
void selectArch(TERM *term, size_t choice);
void loadConfig();
