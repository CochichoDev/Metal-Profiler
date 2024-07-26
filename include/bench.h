#pragma once

#include "global.h"
#include "cli.h"

T_ERROR runBench(TERM *term, size_t iter, RESULT *result_array);
T_VOID processResults(RESULT *result_array, size_t num);

/*
uint64_t benchFullConfig(CoreConfig **config, ttyFD tty);
uint64_t benchIsolationConfig(CoreConfig **config, ttyFD tty);
*/
