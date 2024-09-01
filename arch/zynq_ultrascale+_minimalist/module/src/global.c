#include "global.h"
#include "api.h"

PROP proprieties_core0[13] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .OPTS={"WRITE", "READ", "WRITENEON"}},
    {.NAME="SIZE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={1024,2000000}, .iSTEP=1024},
    {.NAME="STRIDE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={40,256}},
    {.NAME="LIMIT", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={20,10000}},
    {.NAME="CACHECOLORING", .PTYPE=pINT, .FLAGS=MITIGATION, .iRANGE={0,1}},
    {.NAME="MEMBANDWIDTH", .PTYPE=pINT, .FLAGS=MITIGATION, .iRANGE={0,1}}
};

PROP proprieties_core1[13] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={0,0}, .OPTS={"WRITE", "READ", "WRITENEON"}},
    {.NAME="SIZE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={1000,2000000}, .iSTEP=1024},
    {.NAME="STRIDE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={40,256}},
    {.NAME="CACHECOLORING", .PTYPE=pINT, .FLAGS=MITIGATION, .iRANGE={0,1}},
    {.NAME="MEMBANDWIDTH", .PTYPE=pINT, .FLAGS=MITIGATION, .iRANGE={0,1}}
};

PROP proprieties_core2[13] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={0,0}, .OPTS={"WRITE", "READ", "WRITENEON"}},
    {.NAME="SIZE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={1000,2000000}, .iSTEP=1024},
    {.NAME="STRIDE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={40,256}},
    {.NAME="CACHECOLORING", .PTYPE=pINT, .FLAGS=MITIGATION, .iRANGE={0,1}},
    {.NAME="MEMBANDWIDTH", .PTYPE=pINT, .FLAGS=MITIGATION, .iRANGE={0,1}}
};

PROP proprieties_core3[13] = {
    {.NAME="APP", .PTYPE=pSTR, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={0,0}, .OPTS={"WRITE", "READ", "WRITENEON"}},
    {.NAME="SIZE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={1000,2000000}, .iSTEP=1024},
    {.NAME="STRIDE", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={40,256}},
    {.NAME="CACHECOLORING", .PTYPE=pINT, .FLAGS=MITIGATION, .iRANGE={0,1}},
    {.NAME="MEMBANDWIDTH", .PTYPE=pINT, .FLAGS=MITIGATION, .iRANGE={0,1}}
};

PROP proprieties_mem_monitor[3] = {
    {.NAME="BUDGET", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={1000,10000}, .iSTEP=1000},
    {.NAME="PERIOD", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={10,1000}, .iSTEP=100},
    {.NAME="REPLENISHMENT", .PTYPE=pINT, .FLAGS=OPTIMIZABLE|NEEDED, .iRANGE={1000,5000}, .iSTEP=1000}
};

pBUFFER buffer_core0 = {
    .PROPS=proprieties_core0,
    .NUM=4
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

pBUFFER buffer_mem_monitor = {
    .PROPS=proprieties_mem_monitor,
    .NUM=3
};

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

COMP mem_monitor = {
    .ID=4,
    .PBUFFER=&buffer_mem_monitor,
    .NAME="MEMBANDWIDTH"
};

CONFIG ARCH_CONFIG = {
    .COMPS={&core0,&core1,&core2,&core3,&mem_monitor},
    .VICTIM_ID=0,
    .NUM=5
};
