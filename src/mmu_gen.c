#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "mmu_gen.h"
#include "arch.h"
#include "types.h"

#define DEBUG

size_t getPageIdx(uintptr_t addr, T_ULONG pgsize) {
    return addr >> (__builtin_ctz(pgsize));
}

T_ERROR addMMUBlock(MAP_ENTRY *entry, T_UINT level, MEM_MAP *map, MMU *mmu, MMU_Block *prev, T_FLAG terminal) {
    /* If there is no previous block, allocate a new one as the head of the level list */
    if (!prev) {
        MMU_Block *temp = mmu->lvl[level];
        mmu->lvl[level] = calloc(1, sizeof(MMU_Block));
        mmu->lvl[level] = temp;
        mmu->lvl[level]->init = entry->range[0];
        mmu->lvl[level]->size = ceil((double)(entry->range[1] - entry->range[0] + 1) / (double)map->lvls[level]);
        mmu->lvl[level]->map_e = entry;
        mmu->lvl[level]->terminal = terminal;
        
        return 0;
    }
    /* Check if the previous block can be extended to fit the new one */
    /* The previous block address must end in the beginning of new block's address-1 */
    /* If blocks are contiguous, they have a possibility to merge */
    if (prev->init + map->lvls[level] * prev->size == entry->range[0]) {
        /* 
         * In case both the new block and the previous are terminals, they only join if they share
         * the same attributes
         */
        if (terminal && prev->terminal) {
            if (prev->map_e->cc == entry->cc && \
                !strcmp(prev->map_e->attr, entry->attr)) 
            {
                prev->size += ceil((double)(entry->range[1] - entry->range[0] + 1) / (double)map->lvls[level]);
                return 0;
            }
        }
        /* 
         * In case both the new block and the previous are non terminals, they can always join
         */
        if (!terminal && !prev->terminal) {
            prev->size += ceil((double)(entry->range[1] - entry->range[0] + 1) / (double)map->lvls[level]);
            return 0;
        }
    }
    /* In case they are either non contiguous or cannot merge, then allocate a new block */
    MMU_Block *new_block = calloc(1, sizeof(MMU_Block));
    if (prev->next != NULL)
        new_block->next = prev->next;
    prev->next = new_block;
    new_block->init = entry->range[0];
    new_block->size = ceil((double)(entry->range[1] - entry->range[0] + 1) / (double)map->lvls[level]);
    new_block->map_e = entry;
    new_block->terminal = terminal;

    return 0;
}


T_FLAG isMMUBlockAvail(MAP_ENTRY *entry, MEM_MAP *map, MMU *mmu) {
    T_UINT lvl_idx = entry->lvl;

    while (lvl_idx >= 0) {
        MMU_Block *block_ptr = mmu->lvl[lvl_idx];
    
        while (block_ptr != NULL) {
            if (block_ptr->init > entry->range[0]) goto NOT_FOUND;
            if (block_ptr->init + (block_ptr->size * map->lvls[lvl_idx]) > entry->range[0]) {
                if (block_ptr->init + (block_ptr->size * map->lvls[lvl_idx]) <= entry->range[1]) {
                    fprintf(stderr, "Mem range badly specified (%lx - %lx) does not fit in a %ld size block\n", \
                            entry->range[0], entry->range[1], map->lvls[entry->lvl]);
                    goto NOT_FOUND;
                }
                break;
            }

            block_ptr = block_ptr->next;
        }

    #ifdef DEBUG
        printf("Level %d has a block from %lx to %lx\n", lvl_idx, block_ptr->init, block_ptr->init + (block_ptr->size * map->lvls[lvl_idx])-1);
    #endif

        lvl_idx--;
    }

    return TRUE;

NOT_FOUND:
    return FALSE;
}

MMU_Block *getPreviousMMUBlock(MAP_ENTRY *entry, T_UINT level, MEM_MAP *map, MMU *mmu) {
    MMU_Block *block_ptr = mmu->lvl[level];
    MMU_Block *prev = NULL;

    if (block_ptr == NULL) return NULL;

    while (block_ptr != NULL) {
        if (block_ptr->init > entry->range[0]) return prev;
        prev = block_ptr;
        block_ptr = block_ptr->next;
    }
    return prev;
}

MMU_Block *checkMMUBlockAlloc(MAP_ENTRY *entry, T_UINT level, MEM_MAP *map, MMU *mmu) {
    MMU_Block *block_ptr = mmu->lvl[level];

    while (block_ptr != NULL) {
        if (block_ptr->init <= entry->range[0] && \
            block_ptr->init + block_ptr->size * map->lvls[level] > entry->range[1])
        {
            return block_ptr;
        }
    }
    return NULL;
}

T_ERROR recurMMULevelCheck(MAP_ENTRY *entry, T_UINT level, MEM_MAP *map, MMU *mmu) {
    if (level == 0) {
        /* ... */
        return 0;
    }
    return recurMMULevelCheck(entry, level-1, map, mmu);
}

T_ERROR mapToMMU(MEM_MAP *map, MMU *mmu) {

    for (size_t entry_idx = 0; entry_idx < map->num_entries; entry_idx++) {
        MMU_Block *block = NULL;
        /* Check if the current block is already allocated */
        if ((block = checkMMUBlockAlloc(&map->entries[entry_idx], map->entries[entry_idx].lvl, map, mmu))) {
            /* Check if the current allocated block is terminal, if not throw an error */
            if (block->terminal)
                continue;

            fprintf(stderr, "Block (%lx - %lx) already exists and its not terminal\n", \
                    block->init, block->init + block->size*map->lvls[map->entries[entry_idx].lvl]);
            return -1;
        }
        addMMUBlock(&map->entries[entry_idx], map->entries[entry_idx].lvl, map, mmu, \
                    getPreviousMMUBlock(&map->entries[entry_idx], map->entries[entry_idx].lvl, map, mmu), TRUE);

        if (recurMMULevelCheck(&map->entries[entry_idx], map->entries[entry_idx].lvl-1, map, mmu)) {
            fprintf(stderr, "Could not generate MMU structure because of entry (%lx - %lx)\n", \
                    map->entries[entry_idx].range[0], map->entries[entry_idx].range[1]);
            return -1;
        }
    }
    return 0;
}

