#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "mmu_gen.h"
#include "arch.h"
#include "types.h"
#include "global.h"
#include "calc.h"

#define DEBUG

size_t getPageIdx(uintptr_t addr, T_ULONG pgsize) {
    return addr >> (__builtin_ctz(pgsize));
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
        if (block_ptr->init > entry->range[1])
            return prev;
        prev = block_ptr;
        block_ptr = block_ptr->next;
    }
    return prev;
}

MMU_Block *checkMMUBlockAlloc(MAP_ENTRY *entry, T_UINT level, MEM_MAP *map, MMU *mmu) {
    MMU_Block *block_ptr = mmu->lvl[level];

    while (block_ptr != NULL) {
        if (block_ptr->init <= entry->range[0]) {
        #ifdef DEBUG
            printf("Checking block (%lx - %lx) for MMU Level %d\n", \
                    block_ptr->init, block_ptr->init + block_ptr->size * map->lvls[level] -1, level);
        #endif

            if(block_ptr->init + block_ptr->size * map->lvls[level] >= entry->range[1]) {
            #ifdef DEBUG
                printf("Block (%lx - %lx) for MMU Level %d already exists\n", \
                        block_ptr->init, block_ptr->init + block_ptr->size * map->lvls[level] -1, level);
            #endif
                return block_ptr;
            } else {
            #ifdef DEBUG
                printf("Previous block (%lx - %lx) of level %d is not big enough\n", \
                        block_ptr->init, block_ptr->init + block_ptr->size * map->lvls[level] -1, level);
            #endif
                block_ptr = block_ptr->next;
                continue;
            }
        } else {
            return NULL;
        }
    }
    return block_ptr;
}

T_ERROR addMMUBlock(MAP_ENTRY *entry, T_UINT level, MEM_MAP *map, MMU *mmu) {
    MMU_Block *prev = getPreviousMMUBlock(entry, level, map, mmu);
    /* If there is no previous block, allocate a new one as the head of the level list */
    T_FLAG terminal = level == entry->lvl;
    if (!prev) {
    #ifdef DEBUG
        printf("No blocks detected at level %u\n", level);
    #endif
        mmu->lvl[level] = calloc(1, sizeof(MMU_Block));
        mmu->lvl[level]->init = entry->range[0];
        mmu->lvl[level]->size = ceil((double)(entry->range[1] - entry->range[0] + 1) / (double)map->lvls[level]);
        mmu->lvl[level]->map_e = entry;
        mmu->lvl[level]->terminal = terminal;
        
    #ifdef DEBUG
        printf("Allocating first block (%lx - %lx) at level %d\n", \
                mmu->lvl[level]->init, mmu->lvl[level]->init + map->lvls[level]*mmu->lvl[level]->size-1, level);
    #endif
        return 0;
    }
    /* Check if the previous block can be extended to fit the new one */
    /* The previous block address must end in the beginning of new block's address-1 */
    /* If blocks are contiguous, they have a possibility to merge */
    if (prev->init + map->lvls[level] * prev->size >= entry->range[0]) {
        /* 
         * In case both the new block and the previous are terminals, they only join if they share
         * the same attributes
         */
        if (terminal && prev->terminal) {
            if (prev->map_e->cc == entry->cc && \
                !strcmp(prev->map_e->attr, entry->attr)) 
            {
            #ifdef DEBUG
                printf("Increasing previously terminal allocated block (%lx - %lx) at level %u from size %lu to ", \
                        prev->init, prev->init + map->lvls[level]*prev->size-1, level, prev->size);
            #endif
                prev->size += ceil((double)(entry->range[1] - (prev->init + map->lvls[level] * prev->size) + 1) / (double)map->lvls[level]);
            #ifdef DEBUG
                printf("to size %ld\n", prev->size);
            #endif
                return 0;
            }
        }
        /* 
         * In case both the new block and the previous are non terminals, they can always join
         */
        if (!terminal && !prev->terminal) {
        #ifdef DEBUG
            printf("Increasing previously non-terminal allocated block (%lx - %lx) at level %u from size %lu to ", \
                    prev->init, prev->init + map->lvls[level]*prev->size -1, level, prev->size);
        #endif
            prev->size += ceil((double)(entry->range[1] - (prev->init + map->lvls[level] * prev->size) + 1) / (double)map->lvls[level]);
        #ifdef DEBUG
            printf("to size %ld\n", prev->size);
        #endif
            return 0;
        }
    }
    /* In case they are either non contiguous or cannot merge, then allocate a new block */
    MMU_Block *new_block = calloc(1, sizeof(MMU_Block));
    new_block->next = prev->next;
    prev->next = new_block;
    uintptr_t f_prev = prev->init + map->lvls[level] * prev->size;
    new_block->init = (f_prev > entry->range[0]) ? f_prev : entry->range[0];
    new_block->size = ceil((double)(entry->range[1] - new_block->init + 1) / (double)map->lvls[level]);
    new_block->map_e = entry;
    new_block->terminal = terminal;
    #ifdef DEBUG
        printf("Allocating new block (%lx - %lx) at level %d\n", \
                new_block->init, new_block->init + map->lvls[level]*new_block->size-1, level);
    #endif

    return 0;
}

T_ERROR recurMMULevelCheck(MAP_ENTRY *entry, T_UINT level, MEM_MAP *map, MMU *mmu) {
    T_FLAG terminal = level == entry->lvl;
    MMU_Block *block = NULL;

    if ((block = checkMMUBlockAlloc(entry, level, map, mmu))) {
        if (block->terminal != terminal) {
            fprintf(stderr, "Block (%lx - %lx) already exists and doesn't have the required terminality\n", \
                    block->init, block->init + block->size*map->lvls[level]);
            return -1;
        }
    } else {
        addMMUBlock(entry, level, map, mmu);
    }
    if (level == 0) {
        return 0;
    }
    return recurMMULevelCheck(entry, level-1, map, mmu);
}

T_ERROR mapToMMU(MEM_MAP *map, MMU *mmu) {
    for (size_t entry_idx = 0; entry_idx < map->num_entries; entry_idx++) {
    #ifdef DEBUG
        printf("Entry (%lx - %lx) from MMU Level %u\n", map->entries[entry_idx].range[0], map->entries[entry_idx].range[1], map->entries[entry_idx].lvl);
    #endif
        if (recurMMULevelCheck(&map->entries[entry_idx], map->entries[entry_idx].lvl, map, mmu)) {
            fprintf(stderr, "Could not generate MMU structure because of entry (%lx - %lx)\n", \
                    map->entries[entry_idx].range[0], map->entries[entry_idx].range[1]);
            return -1;
        }
    #ifdef DEBUG
        printf("\n\n");
    #endif
    }
    return 0;
}

MMU *createMMU(MEM_MAP *map) {
    MMU *mmu = (MMU *) calloc(1, sizeof(MMU));
    
    mmu->num_lvls = map->num_lvls;
    mmu->lvl = calloc(mmu->num_lvls, sizeof(MMU_Block *));

    return mmu;
}

T_ERROR freeMMU(MMU *mmu) {
    for (size_t lvl_idx = 0; lvl_idx < mmu->num_lvls; lvl_idx++) {
        MMU_Block *block_ptr = mmu->lvl[lvl_idx], *next = NULL;
        while (block_ptr != NULL) {
            next = block_ptr->next;
            free(block_ptr);
            block_ptr = next;
        }
    }
    free(mmu);
    return 0;
}

T_VOID writePageLoop(FILE *fd, size_t rep, T_PSTR base, T_PSTR attr, size_t inc, size_t desc_size) {
    fprintf(fd, ".set SECT, %s\n", base);
    fprintf(fd, ".rept 0x%lx\n", rep);
    fprintf(fd, ".%ldbyte SECT + %s\n", desc_size, attr);
    fprintf(fd, ".set SECT, SECT + 0x%lx\n", inc);
    fprintf(fd, ".endr\n\n");
}

T_VOID writeCCPageLoop(FILE *fd, size_t rep, T_PSTR base, T_PSTR attr, size_t inc, size_t desc_size) {
    size_t contiguous = contiguousPages(&SELECTED_ARCH.desc);

    rep /= (SELECTED_ARCH.desc.NUM_CORES * contiguous);
    for (size_t color_idx = 0; color_idx < SELECTED_ARCH.desc.NUM_CORES; color_idx++) {
        fprintf(fd, ".set SECT, %s + 0x%lx\n", base, contiguous*color_idx*inc);
        fprintf(fd, ".rept 0x%lx\n", rep);
        fprintf(fd, ".rept 0x%lx\n", contiguous);
        fprintf(fd, ".%ldbyte SECT + %s\n", desc_size, attr);
        fprintf(fd, ".set SECT, SECT + 0x%lx\n", inc);
        fprintf(fd, ".endr\n");
        fprintf(fd, ".set SECT, SECT + 0x%lx\n", (contiguous-1)*SELECTED_ARCH.desc.NUM_CORES*inc);
        fprintf(fd, ".endr\n\n");
    }
}

T_ERROR genTranslationTable(MMU *mmu, MEM_MAP *map) {
    FILE *ts = fopen("translation_table.S", "w");
    
    fprintf(ts, "/*\n\
 *Automatically generated by AutoMetalBench\n\
 *Please make sure to define the attributes especified for each descriptor\n\
 */\n\n");

    for (size_t lvl_idx = 0; lvl_idx < mmu->num_lvls; lvl_idx++) {
        MMU_Block *block_ptr = mmu->lvl[lvl_idx];

        fprintf(ts, ".section .mmu_tbl%ld, \"a\"\n\n", lvl_idx);
        size_t own_block_idx = 0, non_terminal_idx = 0;
        while (block_ptr != NULL) {
            fprintf(ts, "MMUTableL%ld.%ld:\n", lvl_idx, own_block_idx);
            T_STR base;
            T_STR attr;
            size_t inc;
            if (block_ptr->terminal) {
                sprintf(base, "0x%lx", block_ptr->init);
                sprintf(attr, "%s", block_ptr->map_e->attr);
                inc = map->lvls[lvl_idx];
            }
            else {
                sprintf(base, "MMUTableL%ld.%ld", lvl_idx+1, non_terminal_idx++);
                sprintf(attr, "%s", "NEXT_PAGE_ATTR");
                inc = (map->lvls[lvl_idx] / map->lvls[lvl_idx+1]) * map->desc_size;

            }

            if (block_ptr->terminal && block_ptr->map_e->cc)
                writeCCPageLoop(ts, block_ptr->size, base, attr, \
                               inc, map->desc_size);
            else
                writePageLoop(ts, block_ptr->size, base, attr, \
                               inc, map->desc_size);

            block_ptr = block_ptr->next;
            own_block_idx++;
        }
    }

    fclose(ts);
    return 0;
}

T_ERROR genLinkerSkeleton(MMU *mmu, MEM_MAP *map) {
    FILE *ls;
    char file_name[64];

    for (size_t app_idx = 0; app_idx < SELECTED_ARCH.desc.NUM_CORES; app_idx++) {
        sprintf(file_name, "lscript%ld.ld", app_idx);
        ls = fopen(file_name, "w");

        fprintf(ls, "/*\n"
                    " * Automatically generated by AutoMetalBench\n"
                    " * Please make sure to define the attributes especified for each descriptor\n"
                    " */\n\n");

        /* Memory regions generation */
        fprintf(ls, "MEMORY\n"
                    "{\n");
        for (size_t entry_idx = 0; entry_idx < map->num_entries; entry_idx++) {
            size_t entry_size = map->entries[entry_idx].range[1] - map->entries[entry_idx].range[0] + 1;
            fprintf(ls, "\tMEM_REGION_%ld", entry_idx);
            if (map->entries[entry_idx].cc) fprintf(ls, "_CC");
            if (entry_idx == map->shared_section) {
                fprintf(ls, "_SHARED");
            } else if (entry_idx == map->load_section) {
                fprintf(ls, "_LOAD");
                goto DIVIDE_ADDRESS;
            } else if (entry_idx == map->link_section) {
                fprintf(ls, "_LINK");
                goto DIVIDE_ADDRESS;
            } else if (entry_idx == map->boot_section) {
                fprintf(ls, "_BOOT");
                goto DIVIDE_ADDRESS;
            }
            fprintf(ls, " :\t ORIGIN = 0x%lx, LENGTH = 0x%lx\n", map->entries[entry_idx].range[0], \
                        entry_size);
            continue;
        DIVIDE_ADDRESS:
            fprintf(ls, " :\t ORIGIN = 0x%lx, LENGTH = 0x%lx\n", \ 
                    map->entries[entry_idx].range[0] + app_idx * (entry_size / (SELECTED_ARCH.desc.NUM_CORES)), \
                    entry_size / (SELECTED_ARCH.desc.NUM_CORES));

        }
        fprintf(ls, "}\n\n");

        fprintf(ls, "ENTRY(_boot)\n\n");

        /* Section generation */
        fprintf(ls, "SECTIONS\n"
                    "{\n");
        

        // MMU section
        for (size_t tbl_idx = 0; tbl_idx < mmu->num_lvls; tbl_idx++) {
            fprintf(ls, ".mmu_tbl%ld (NOLOAD) : {\n", tbl_idx);
            fprintf(ls, "\t. = ALIGN(%ld);\n", map->lvls[map->num_lvls-1]);
            fprintf(ls, "\t__mmu_tbl%ld_start = .;\n", tbl_idx);
            fprintf(ls, "\t*(.mmu_tbl%ld)\n", tbl_idx);
            fprintf(ls, "\t__mmu_tbl%ld_end = .;\n", tbl_idx);
            fprintf(ls, "} > MEM_REGION_%ld%s_SHARED\n", \
                    map->shared_section, \
                    (map->entries[map->shared_section].cc) ? "_CC" : "");
        }

        fprintf(ls, "\n. = ALIGN(64);\n");
        fprintf(ls, "__loader_vma = .;\n\n");

        // Boot section
        fprintf(ls, ".boot : {\n");
        fprintf(ls, "\t. = ALIGN(64);\n");
        fprintf(ls, "\t__boot_start = .;\n");
        fprintf(ls, "\t*(.boot)\n");
        fprintf(ls, "\t__boot_end = .;\n");
        fprintf(ls, "} > MEM_REGION_%ld_BOOT\n", map->boot_section);

        // Text section
        fprintf(ls, ".text : {\n");
        fprintf(ls, "\t__ld_text = LOADADDR(.text);\n");
        fprintf(ls, "\t__text_start = .;\n");
        fprintf(ls, "\t. = ALIGN(64);\n");
        fprintf(ls, "\tKEEP (*(.vectors))\n");
        fprintf(ls, "\t*(.handlers)\n");
        fprintf(ls, "\t*(.text)\n");
        fprintf(ls, "\t*(.text)\n");
        fprintf(ls, "\t*(.text.*)\n");
        fprintf(ls, "\t__text_end = .;\n");
        fprintf(ls, "} > MEM_REGION_%ld%s_LINK AT > MEM_REGION_%ld%s_LOAD\n", \
                map->link_section, \
                (map->entries[map->link_section].cc) ? "_CC" : "", \
                map->load_section, \
                (map->entries[map->load_section].cc) ? "_CC" : "");

        // ROData section
        fprintf(ls, ".rodata : {\n");
        fprintf(ls, "\t__ld_rodata = LOADADDR(.rodata);\n");
        fprintf(ls, "\t__rodata_start = .;\n");
        fprintf(ls, "\t. = ALIGN(64);\n");
        fprintf(ls, "\t*(.rodata)\n");
        fprintf(ls, "\t*(.rodata.*)\n");
        fprintf(ls, "\t__rodata_end = .;\n");
        fprintf(ls, "} > MEM_REGION_%ld%s_LINK AT > MEM_REGION_%ld%s_LOAD\n", \
                map->link_section, \
                (map->entries[map->link_section].cc) ? "_CC" : "", \
                map->load_section, \
                (map->entries[map->load_section].cc) ? "_CC" : "");

        // Data section
        fprintf(ls, ".data : {\n");
        fprintf(ls, "\t__ld_data = LOADADDR(.data);\n");
        fprintf(ls, "\t__data_start = .;\n");
        fprintf(ls, "\t. = ALIGN(64);\n");
        fprintf(ls, "\t*(.data)\n");
        fprintf(ls, "\t*(.data.*)\n");
        fprintf(ls, "\t__data_end = .;\n");
        fprintf(ls, "} > MEM_REGION_%ld%s_LINK AT > MEM_REGION_%ld%s_LOAD\n", \
                map->link_section, \
                (map->entries[map->link_section].cc) ? "_CC" : "", \
                map->load_section, \
                (map->entries[map->load_section].cc) ? "_CC" : "");
        
        // Stack section
        fprintf(ls, ".stack : {\n");
        fprintf(ls, "\t. = ALIGN(64);\n");
        fprintf(ls, "\t__stack_start = .;\n");
        fprintf(ls, "\t. += _STACK_SIZE;\n");
        fprintf(ls, "\t__stack_end = .;\n");
   
        fprintf(ls, "} > MEM_REGION_%ld%s_LINK\n", \
                map->link_section, \
                (map->entries[map->link_section].cc) ? "_CC" : "");

        // BSS section
        fprintf(ls, ".bss : {\n");
        fprintf(ls, "\t. = ALIGN(64);\n");
        fprintf(ls, "\t__bss_start = .;\n");
        fprintf(ls, "\t*(.bss)\n");
        fprintf(ls, "\t*(.bss.*)\n");
        fprintf(ls, "\t*(.gnu.linkonce.b.*)\n");
        fprintf(ls, "\t*(COMMON)\n");
        fprintf(ls, "\t. = ALIGN(64);\n");
        fprintf(ls, "\t__bss_end = .;\n");
        fprintf(ls, "} > MEM_REGION_%ld%s_LINK\n", \
                map->link_section, \
                (map->entries[map->link_section].cc) ? "_CC" : "");

        // SBSS section
        fprintf(ls, ".sbss : {\n");
        fprintf(ls, "\t. = ALIGN(64);\n");
        fprintf(ls, "\t__sbss_start = .;\n");
        fprintf(ls, "\t*(.sbss)\n");
        fprintf(ls, "\t*(.sbss.*)\n");
        fprintf(ls, "\t*(.gnu.linkonce.sb.*)\n");
        fprintf(ls, "\t. = ALIGN(64);\n");
        fprintf(ls, "\t__sbss_end = .;\n");
        fprintf(ls, "} > MEM_REGION_%ld%s_LINK\n", \
                map->link_section, \
                (map->entries[map->link_section].cc) ? "_CC" : "");
        
        // Buffer section (usefull only for the benchmark)
        fprintf(ls, "/* This section is only useful for the standard benchmarks */\n");
        fprintf(ls, ".buffer : {\n");
        fprintf(ls, "\t. = ALIGN(64);\n");
        fprintf(ls, "\t__buffer_start = .;\n");
        fprintf(ls, "\t. += _BUFFER_SIZE;\n");
        fprintf(ls, "\t__buffer_end = .;\n");
        fprintf(ls, "} > MEM_REGION_%ld%s_LINK\n", \
                map->link_section, \
                (map->entries[map->link_section].cc) ? "_CC" : "");
        fprintf(ls, "}\n");
        fclose(ls);
    }

    return 0;
}

T_ERROR genMMU(ARCH *arch) {
    assert(arch->desc.CACHES != NULL && arch->map.entries != NULL);
    MMU *mmu = createMMU(&arch->map);
    mapToMMU(&SELECTED_ARCH.map, mmu);
    genTranslationTable(mmu, &SELECTED_ARCH.map);
    genLinkerSkeleton(mmu, &SELECTED_ARCH.map);

    freeMMU(mmu);
    return 0;
}

T_ERROR genLinker(ARCH *arch) {
    assert(arch->desc.CACHES != NULL && arch->map.entries != NULL);
    MMU *mmu = createMMU(&arch->map);
    mapToMMU(&SELECTED_ARCH.map, mmu);
    genLinkerSkeleton(mmu, &SELECTED_ARCH.map);

    freeMMU(mmu);
    return 0;
}
