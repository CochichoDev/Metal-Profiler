#pragma once

#include <stdint.h>
#include <unistd.h>

#include "common.h"

err cliStart();
err cliClear();
err cliGetInput();
err cliClose();
void cliPrintProgress(size_t cur, size_t max);

