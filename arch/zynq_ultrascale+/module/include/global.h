#pragma once

#include "api.h"

#include <stdint.h>

#define PROJDIR     BASEDIR"zynq_ultrascale+/project"

#define MAKEFILE0   PROJDIR"/Core0/Debug"
#define MAKEFILE1   PROJDIR"/Core1/Debug"

#define NUM_CORES    4

#define NUM_CONF    13

typedef struct {
    INT mode;               // ENEMY:   0 ; VICTIM: 1
    STR application;
    INT times;
    INT target_size;
    INT stride;
    INT limit;
} CoreConfig;

extern CONFIG ARCH_CONFIG;
