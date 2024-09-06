#ifndef ARCH_H
#define ARCH_H

#include <stddef.h>

#include "types.h"

typedef struct {
    T_ULONG     range[2];
    T_UINT      lvl;
    T_STR       attr;
    T_FLAG      cc;
} MAP_ENTRY;

typedef struct {
    T_UINT      desc_size;
    T_ULONG    *lvls;
    MAP_ENTRY  *entries;
    size_t      num_entries;
} MEM_MAP;

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
