#pragma once

#include "global.h"

T_ERROR runBench(size_t iter, RESULT *result_array);
T_VOID processResults(G_ARRAY *result_data);

T_VOID computeInterferenceDegradation(G_ARRAY *garrays_std_input, size_t num_garrays, G_ARRAY *garrays_std_deg);
T_VOID computeDegradation(G_ARRAY *garrays_std_input, size_t num_garrays, G_ARRAY *garrays_std_deg, CONFIG *cfg);

/*
uint64_t benchFullConfig(CoreConfig **config, ttyFD tty);
uint64_t benchIsolationConfig(CoreConfig **config, ttyFD tty);
*/
