#pragma once

#include "global.h"

T_ERROR runBench(size_t iter, RESULT *result_array);
T_VOID processResults(G_ARRAY *result_data);

/*
uint64_t benchFullConfig(CoreConfig **config, ttyFD tty);
uint64_t benchIsolationConfig(CoreConfig **config, ttyFD tty);
*/
