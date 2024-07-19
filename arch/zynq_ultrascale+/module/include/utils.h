#pragma once

#include <sys/wait.h>

#include "global.h"

#define MAKEFILE_PATH "Core%d/Debug"

void callMakefiles(void **config);
pid_t launchProcess(const char *path, ...);
const char *makeString(CoreConfig *config);
