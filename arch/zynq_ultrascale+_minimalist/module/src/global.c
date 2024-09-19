#include "AMBapi.h"

PROP proprieties_system[5] = {
    {.NAME="CACHECOLORING", .PTYPE=pBOOL, .FLAGS=MITIGATION, .iRANGE={0,1}},
    {.NAME="MEMBANDWIDTH", .PTYPE=pBOOL, .FLAGS=MITIGATION, .iRANGE={0,1}},
    {.NAME="BUDGET", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={100,500}, .iSTEP=100},
    {.NAME="PERIOD", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={1,100}, .iSTEP=1},
    {.NAME="REPLENISHMENT", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={5,40}, .iSTEP=5}
};

PROP proprieties_core1[4] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .OPTS={"WRITE", "READ", "WRITENEON"}},
    {.NAME="SIZE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={1024,2000000}, .iSTEP=1024},
    {.NAME="STRIDE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={40,256}},
    {.NAME="LIMIT", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={20,10000}},
};

PROP proprieties_core2[3] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={0,0}, .OPTS={"WRITE", "READ", "WRITENEON"}},
    {.NAME="SIZE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={1000,2000000}, .iSTEP=1024},
    {.NAME="STRIDE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={40,256}},
};

PROP proprieties_core3[3] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={0,0}, .OPTS={"WRITE", "READ", "WRITENEON"}},
    {.NAME="SIZE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={1000,2000000}, .iSTEP=1024},
    {.NAME="STRIDE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={40,256}},
};

PROP proprieties_core4[3] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={0,0}, .OPTS={"WRITE", "READ", "WRITENEON"}},
    {.NAME="SIZE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={1000,2000000}, .iSTEP=1024},
    {.NAME="STRIDE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={40,256}},
};

pBUFFER buffer_system = {
    .PROPS=proprieties_system,
    .NUM=5
};

pBUFFER buffer_core1 = {
    .PROPS=proprieties_core1,
    .NUM=4
};

pBUFFER buffer_core2 = {
    .PROPS=proprieties_core2,
    .NUM=3
};

pBUFFER buffer_core3 = {
    .PROPS=proprieties_core3,
    .NUM=3
};

pBUFFER buffer_core4 = {
    .PROPS=proprieties_core4,
    .NUM=3
};


COMP comp_system = {
    .ID=0,
    .NAME="SYSTEM",
    .PBUFFER=&buffer_system
};

COMP core1 = {
    .ID=1,
    .NAME="CORE1",
    .PBUFFER=&buffer_core1
};

COMP core2 = {
    .ID=2,
    .NAME="CORE2",
    .PBUFFER=&buffer_core2
};

COMP core3 = {
    .ID=3,
    .NAME="CORE3",
    .PBUFFER=&buffer_core3
};

COMP core4 = {
    .ID=4,
    .NAME="CORE4",
    .PBUFFER=&buffer_core4
};

CONFIG ARCH_CONFIG = {
    .COMPS={&comp_system,&core1,&core2,&core3,&core4},
    .VICTIM_ID=1,
    .NUM=5
};
