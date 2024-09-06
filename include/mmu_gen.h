#ifndef MMU_GEN_H
#define MMU_GEN_H

#include "types.h"
#include "arch.h"

typedef struct mmu_block_s {
    T_ULONG init;
    size_t size;
    MAP_ENTRY *map_e;
    T_FLAG terminal;
    struct mmu_block_s *next;
} MMU_Block;

typedef struct {
    MMU_Block **lvl;
    size_t num_lvls;
} MMU;

#endif
