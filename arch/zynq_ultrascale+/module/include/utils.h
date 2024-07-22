#pragma once

#include <sys/wait.h>

#include "api.h"

void callMakefiles(CONFIG *config);
pid_t launchProcess(const char *path, ...);
void makeString(COMP *comp, STR_P CFLAGS);
