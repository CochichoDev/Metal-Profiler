#pragma once

#include "api.h"

#include <stdint.h>

#define PROJDIR     BASEDIR"zynq_ultrascale+_minimalist/project"

#define T32SCRIPT   PROJDIR"/launch_bench.cmm"
#define TTY_PORT    "/dev/ttyUSB0"

#define MAKEFILE_PATH   PROJDIR"/Core%d"

#define NUM_CORES    4

#define NUM_CONF    13


extern CONFIG ARCH_CONFIG;
extern CONFIG *CUR_CONFIG;
