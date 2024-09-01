#pragma once

#include <sys/wait.h>

#include "api.h"

void callMakefiles(CONFIG *config);
void makeString(COMP *comp, T_PSTR CFLAGS);
