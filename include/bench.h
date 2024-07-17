#pragma once

#include "global.h"
#include "tty.h"

uint64_t benchFullConfig(CoreConfig **config, ttyFD tty);
uint64_t benchIsolationConfig(CoreConfig **config, ttyFD tty);
