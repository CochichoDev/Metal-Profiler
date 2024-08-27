#include "api.h"

T_ERROR addOutputOption(T_PSTR graph, T_PSTR data, T_PSTR name, size_t size, TYPE type);
T_VOID listSelectedOutputOptions();
T_ERROR deleteOutputOption(T_PSTR name);

T_ERROR cleanState();
T_VOID destroyConfig(CONFIG *cfg);
