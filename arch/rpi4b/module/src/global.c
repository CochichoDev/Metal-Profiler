#include "AMBapi.h"

PROP proprieties_system[5] = {
    {.NAME="CACHECOLORING", .PTYPE=pBOOL, .FLAGS=MITIGATION, .iRANGE={0,1}},
    {.NAME="MEMBANDWIDTH", .PTYPE=pBOOL, .FLAGS=MITIGATION, .iRANGE={0,1}},
    {.NAME="BUDGET", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={200,8000}, .iSTEP=200},
    {.NAME="PERIOD", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={50,500}, .iSTEP=50},
    {.NAME="THRESHOLD", .PTYPE=pDOUBLE, .FLAGS=OPTIMIZABLE|NEEDED, .fRANGE={0.01f,0.5f}, .fSTEP=0.01f}
};

PROP proprieties_core1[4] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .OPTS={"WRITE", "READ", "WRITENEON", "READNEON"}},
    {.NAME="SIZE", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .OPTS={"L1","L2", "DDR"}},
    {.NAME="STRIDE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={40,256}},
    {.NAME="ITERATIONS", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={20,20000}}
};

PROP proprieties_core2[4] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .OPTS={"WRITE", "READ", "WRITENEON", "READNEON"}},
    {.NAME="SIZE", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .OPTS={"L1","L2", "DDR"}},
    {.NAME="STRIDE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={40,256}},
    {.NAME="ITERATIONS", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={20,10000}}
};

PROP proprieties_core3[4] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .OPTS={"WRITE", "READ", "WRITENEON", "READNEON"}},
    {.NAME="SIZE", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .OPTS={"L1","L2", "DDR"}},
    {.NAME="STRIDE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={40,256}},
    {.NAME="ITERATIONS", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={20,10000}}
};

PROP proprieties_core4[4] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .OPTS={"WRITE", "READ", "WRITENEON", "READNEON"}},
    {.NAME="SIZE", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .OPTS={"L1","L2", "DDR"}},
    {.NAME="STRIDE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={40,256}},
    {.NAME="ITERATIONS", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={20,10000}}
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
    .NUM=4
};

pBUFFER buffer_core3 = {
    .PROPS=proprieties_core3,
    .NUM=4
};

pBUFFER buffer_core4 = {
    .PROPS=proprieties_core4,
    .NUM=4
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
