#include "global.h"
#include "api.h"

PROP proprieties_core0[13] = {
    {.NAME="APP", .PTYPE=pSTR, .NEED=1, .iRANGE={0,0}, .iINIT=0, .OPTS={"WRITE", "READ"}},
    {.NAME="SIZE", .PTYPE=pINT, .NEED=1, .iRANGE={1000,260000}, .iINIT=0, .OPTS=NULL},
    {.NAME="STRIDE", .PTYPE=pINT, .NEED=1, .iRANGE={40,128}, .iINIT=0, .OPTS=NULL},
    {.NAME="LIMIT", .PTYPE=pINT, .NEED=1, .iRANGE={20,50}, .iINIT=0, .OPTS=NULL}
};

PROP proprieties_core1[13] = {
    {.NAME="APP", .PTYPE=pSTR, .NEED=1, .iRANGE={0,0}, .iINIT=0, .OPTS={"WRITE", "READ"}},
    {.NAME="SIZE", .PTYPE=pINT, .NEED=1, .iRANGE={1000,260000}, .iINIT=0, .OPTS=NULL},
    {.NAME="STRIDE", .PTYPE=pINT, .NEED=1, .iRANGE={40,128}, .iINIT=0, .OPTS=NULL}
};

PROP proprieties_core2[13] = {
    {.NAME="APP", .PTYPE=pSTR, .NEED=1, .iRANGE={0,0}, .iINIT=0, .OPTS={"WRITE", "READ"}},
    {.NAME="SIZE", .PTYPE=pINT, .NEED=1, .iRANGE={1000,260000}, .iINIT=0, .OPTS=NULL},
    {.NAME="STRIDE", .PTYPE=pINT, .NEED=1, .iRANGE={40,128}, .iINIT=0, .OPTS=NULL}
};

PROP proprieties_core3[13] = {
    {.NAME="APP", .PTYPE=pSTR, .NEED=1, .iRANGE={0,0}, .iINIT=0, .OPTS={"WRITE", "READ"}},
    {.NAME="SIZE", .PTYPE=pINT, .NEED=1, .iRANGE={1000,260000}, .iINIT=0, .OPTS=NULL},
    {.NAME="STRIDE", .PTYPE=pINT, .NEED=1, .iRANGE={40,128}, .iINIT=0, .OPTS=NULL}
};

pBUFFER buffer_core0 = {
    .PROPS=proprieties_core0,
    4
};

pBUFFER buffer_core1 = {
    .PROPS=proprieties_core1,
    3
};

pBUFFER buffer_core2 = {
    .PROPS=proprieties_core2,
    3
};

pBUFFER buffer_core3 = {
    .PROPS=proprieties_core3,
    3
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

CONFIG ARCH_CONFIG = {
    .COMPS={&core0,&core1,&core2,&core3},
    .NUM=4
};
