#ifndef MMU_GEN_H
#define MMU_GEN_H

#include "types.h"
#include "arch.h"
#include "state.h"

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

T_ERROR mapToMMU(MEM_MAP *map, MMU *mmu);
MMU *createMMU(MEM_MAP *map);
T_ERROR freeMMU(MMU *mmu);
T_ERROR genTranslationTable(MMU *mmu, MEM_MAP *map);
T_ERROR genLinkerSkeleton(MMU *mmu, MEM_MAP *map);
T_ERROR genMMU(ARCH *arch);
T_ERROR genLinker(ARCH *arch);
#endif
