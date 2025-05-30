#include "cache_controller.h"

#ifdef A72
void disable_no_allocate() {
	register uint64_t value = 0;

	__asm__ __volatile__("MRS %0, S3_1_C15_C2_0" : "=r"(value));
	__asm__ __volatile__("MSR S3_1_C15_C2_0, %0" : : "r" ((value & CPUACTLR_MASK) | CPUACTLR_NO_ALLOCATE_DISABLE));
}

void no_allocate_threshold_L1(uint32_t mode) {
	register uint64_t value = 0;

	__asm__ __volatile__("MRS %0, S3_1_C15_C2_0" : "=r"(value));
	__asm__ __volatile__("MSR S3_1_C15_C2_0, %0" : : "r" ((value & CPUACTLR_MASK) | (mode << 25)));
}
#else
/* no_allocate_threshold_*
 *
 * Description: Changes the configuration on the no allocate functionality of BIU regarding L2 Cache
 *
 * Parameter:
 * 				- u8 mode: Selects the mode of configuration of this functionality:
 * 						1. 0b00: Default value, no allocate every 4th consecutive cache line
 * 						2. 0b01: no allocate the 64th consecutive cache line write miss
 * 						3. 0b10: no allocate the 128th consecutive cache line write miss
 * 						4. 0b11: disables the no allocate feature
 *
 * Returns:		Nothing
 *
 */
void no_allocate_threshold_L1(uint32_t mode) {
	register uint64_t value = 0;

	__asm__ __volatile__("MRS %0, S3_1_C15_C2_0" : "=r"(value));
	__asm__ __volatile__("MSR S3_1_C15_C2_0, %0" : : "r" ((value & CPUACTLR_MASK) | (mode << 25)));
}

void no_allocate_threshold_L2(uint32_t mode) {
	register uint64_t value = 0;

	__asm__ __volatile__("MRS %0, S3_1_C15_C2_0" : "=r"(value));
	__asm__ __volatile__("MSR S3_1_C15_C2_0, %0" : : "r" ((value & CPUACTLR_MASK2) | (mode << 27)));
}
#endif

#ifdef A72
void disable_outstanding_prefetching() {
	register uint64_t value = 0;

	__asm__ __volatile__("MRS %0, S3_1_C15_C2_1" : "=r"(value));
	__asm__ __volatile__("MSR S3_1_C15_C2_1, %0" : : "r" ((value & CPUECTLR_PREFETCH_MASK) | CPUECTLR_PREFETCH_DISABLE));
	__asm__ __volatile__("MRS %0, S3_1_C15_C2_0" : "=r"(value));
	__asm__ __volatile__("MSR S3_1_C15_C2_0, %0" : : "r" ((value & CPUACTLR_PREFETCH_MASK) | CPUACTLR_PREFETCH_DISABLE));
}
void disable_va_based_prefetching() {
	register uint64_t value = 0;

	__asm__ __volatile__("MRS %0, S3_1_C15_C2_0" : "=r"(value));
	__asm__ __volatile__("MSR S3_1_C15_C2_0, %0" : : "r" ((value & CPUACTLR_VA_PREFETCH_MASK) | CPUACTLR_VA_PREFETCH_DISABLE));
}
#else
void set_outstanding_prefetching(uint8_t L1PCTL) {
	register uint64_t value = 0;

	__asm__ __volatile__("MRS %0, S3_1_C15_C2_0" : "=r"(value));
	__asm__ __volatile__("MSR S3_1_C15_C2_0, %0" : : "r" ((value & CPUACTLR_PREFETCH_MASK) | ((L1PCTL & 0x7U) << 13)));
}
#endif
