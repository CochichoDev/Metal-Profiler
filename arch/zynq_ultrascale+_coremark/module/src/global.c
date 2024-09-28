#include "AMBapi.h"

PROP proprieties_system[5] = {
    {.NAME="CACHECOLORING", .PTYPE=pBOOL, .FLAGS=0, .iRANGE={0,1}},
    {.NAME="MEMBANDWIDTH", .PTYPE=pBOOL, .FLAGS=0, .iRANGE={0,1}},
    {.NAME="INIT_BUDGET", .PTYPE=pINT, .FLAGS=0, .iRANGE={200,1000}},
    {.NAME="PERIOD", .PTYPE=pINT, .FLAGS=0, .iRANGE={1,20}},
    {.NAME="REPLENISHMENT", .PTYPE=pINT, .FLAGS=0, .iRANGE={10,100}}
};

PROP proprieties_core1[6] = {
    {.NAME="ITERATIONS", .PTYPE=pINT, .FLAGS=NEEDED, .iRANGE={1,1000000}},
    {.NAME="VALIDATION_RUN", .PTYPE=pBOOL, .FLAGS=0, .iRANGE={0,1}},
    {.NAME="PROFILE_RUN", .PTYPE=pBOOL, .FLAGS=0, .iRANGE={0,1}},
    {.NAME="PERFORMANCE_RUN", .PTYPE=pBOOL, .FLAGS=0, .iRANGE={0,1}},
    {.NAME="MEM_METHOD", .PTYPE=pSTR, .FLAGS=0, .OPTS={"MEM_STACK", "MEM_STATIC", "MEM_MALLOC"}},
    {.NAME="TOTAL_DATA_SIZE", .PTYPE=pINT, .FLAGS=0, .iRANGE={1000,1500000}}
};

PROP proprieties_core2[3] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={0,0}, .OPTS={"WRITE", "READ", "WRITENEON"}},
    {.NAME="SIZE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={150000,1500000}, .iSTEP=1024},
    {.NAME="STRIDE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={40,256}},
};

PROP proprieties_core3[3] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={0,0}, .OPTS={"WRITE", "READ", "WRITENEON"}},
    {.NAME="SIZE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={150000,1500000}, .iSTEP=1024},
    {.NAME="STRIDE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={40,256}},
};

PROP proprieties_core4[3] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={0,0}, .OPTS={"WRITE", "READ", "WRITENEON"}},
    {.NAME="SIZE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={150000,1500000}, .iSTEP=1024},
    {.NAME="STRIDE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={40,256}},
};

pBUFFER buffer_system = {
    .PROPS=proprieties_system,
    .NUM=5
};

pBUFFER buffer_core1 = {
    .PROPS=proprieties_core1,
    .NUM=6
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

COMP comp_core1 = {
    .ID=1,
    .NAME="CORE1",
    .PBUFFER=&buffer_core1
};

COMP comp_core2 = {
    .ID=2,
    .NAME="CORE2",
    .PBUFFER=&buffer_core2
};

COMP comp_core3 = {
    .ID=3,
    .NAME="CORE3",
    .PBUFFER=&buffer_core3
};

COMP comp_core4 = {
    .ID=4,
    .NAME="CORE4",
    .PBUFFER=&buffer_core4
};

CONFIG ARCH_CONFIG = {
    .COMPS={&comp_system,&comp_core1,&comp_core2,&comp_core3,&comp_core4},
    .VICTIM_ID=1,
    .NUM=5
};
