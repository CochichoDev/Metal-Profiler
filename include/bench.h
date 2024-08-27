#pragma once

#include "global.h"

T_ERROR runBench(size_t iter, T_UINT numResults, RESULT **results_input);
T_VOID processResults(G_ARRAY *garray_result_input, size_t num_outputs, OUTPUT **output_array);

T_VOID computeInterferenceDegradation(G_ARRAY *garrays_std_input, size_t num_garrays, G_ARRAY *garrays_std_deg);
T_VOID computeProprietyDegradation(G_ARRAY *garrays_std_input, size_t num_garrays, G_ARRAY *garrays_std_deg);
T_VOID computeDegradation(G_ARRAY *garrays_std_input, size_t num_garrays, G_ARRAY *garrays_std_deg, CONFIG *cfg);

T_VOID analysisTUI();
/*
uint64_t benchFullConfig(CoreConfig **config, ttyFD tty);
uint64_t benchIsolationConfig(CoreConfig **config, ttyFD tty);
*/
