#ifndef CACHE_CONTROLLER_H
#define CACHE_CONTROLLER_H

#include <stdint.h>

#include "bspconfig.h"

#ifdef A72
#define CPUACTLR_MASK	        (0xFFFDFFFFE7FFFFFF)
#define CPUACTLR_NO_ALLOCATE_DISABLE	    (0x000240001E000000)
#else
#define CPUACTLR_MASK	        (0xF9FFFFFF)
#define CPUACTLR_MASK2	        (0xE7FFFFFF)
#endif
#ifdef A72
#define CPUECTLR_PREFETCH_MASK      (0xFFFFFFA7FFFFFFFF)
#define CPUECTLR_PREFETCH_DISABLE   (0x0000004000000000)
#define CPUACTLR_PREFETCH_MASK      (0xFEFF7BFEFFDFFFFF)
#define CPUACTLR_PREFETCH_DISABLE   (0x0100840100200000)
#define CPUACTLR_VA_PREFETCH_MASK   (0xFFFFF7FFFFFFFFFF)
#define CPUACTLR_VA_PREFETCH_DISABLE   (0x0000080000000000)
#else
#define CPUACTLR_PREFETCH_MASK  (0xFFFF1FFF)
#endif

#ifdef A72
void disable_no_allocate();
void disable_va_based_prefetching();
void disable_outstanding_prefetching();
#else
void no_allocate_threshold_L1(uint32_t mode);
void no_allocate_threshold_L2(uint32_t mode);
void set_outstanding_prefetching(uint8_t L1PCTL);
#endif


#endif
