#ifndef ARCH_H
#define ARCH_H

#include <stddef.h>

#include "types.h"

typedef struct {
    T_UINT      LINE_SIZE;
    T_UINT      SIZE;
    T_UINT      WAYS;
    T_UINT      SHARED_NUM;
} CACHE_DESC;

typedef struct {
    T_UINT      NUM_CORES;
    T_UINT      PAGE_SIZE;
    CACHE_DESC *CACHES;
    T_UINT      CACHE_LVL;
} ARCH_DESC;

#endif
