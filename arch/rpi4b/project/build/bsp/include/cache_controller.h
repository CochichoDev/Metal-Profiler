#ifndef CACHE_CONTROLLER_H
#define CACHE_CONTROLLER_H

#include <stdint.h>

#define CPUACTLR_MASK	        (0xF9FFFFFF)
#define CPUACTLR_MASK2	        (0xE7FFFFFF)
#define CPUACTLR_PREFETCH_MASK  (0xFFFF1FFF)

void no_allocate_threshold_L1(uint32_t mode);
void no_allocate_threshold_L2(uint32_t mode);
void set_outstanding_prefetching(uint8_t L1PCTL);

#endif
