#include "global.h"
#include "api.h"

PROP proprieties_core0[2] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=NEEDED|OPTIMIZABLE, .iRANGE={0,0}, .iINIT=0, .OPTS={"FLC", "LLC", "DRAM", NULL}},
    {.NAME="LIMIT", .PTYPE=pINT, .FLAGS=NEEDED, .iRANGE={10,10000}, .iINIT=0, .OPTS={NULL}, .iSTEP=10}
};

PROP proprieties_core1[1] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=NEEDED|OPTIMIZABLE, .iRANGE={0,0}, .iINIT=0, .OPTS={"FLC", "LLC", "DRAM", NULL}},
};

pBUFFER buffer_core0 = {.PROPS=proprieties_core0, .NUM=2};
pBUFFER buffer_core1 = {.PROPS=proprieties_core1, .NUM=1};

COMP core0 = {
    .ID=0,
    .NAME="CORE0",
    .PBUFFER=&buffer_core0
};
COMP core1 = {
    .ID=1,
    .NAME="CORE1",
    .PBUFFER=&buffer_core1
};


CONFIG ARCH_CONFIG = {
    .COMPS={&core0,&core1},
    .VICTIM_ID=0,
    .NUM=2
};
