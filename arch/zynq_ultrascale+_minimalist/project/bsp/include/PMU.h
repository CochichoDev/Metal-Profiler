#ifndef PMU_H
#define PMU_H

#include <stdint.h>

#define PMC_CNTR_RESET_BIT      (2)
#define PMC_EVENT_RESET_BIT     (1)
#define PMC_ENABLE_BIT          (0)

#define PMCNTENSET_CNTR_BIT     (31)

#define PMCNTENCLR_CNTR_BIT     (31)

#define PMSELR_MASK             (0x1FU)

#define PMEVTYPER_MASK          (0xFFFFU)

/****************EVENTS*****************/
#define L1D_CACHE_REFILL    (0x03U)
#define L1D_CACHE_WB        (0x15U)
#define L2_CACHE_REFILL     (0x17U)
#define L2_CACHE_WB         (0x18U)
#define BUS_CYCLES          (0x1DU)


/*****************INLINE FUNCTION DEFINITION**************/
static __attribute__((always_inline)) void reset_pmccntr() {
    register uint64_t pmcr_value;

    __asm__ __volatile__(
                        "MRS    %0,     PMCR_EL0"
                        : "=r" (pmcr_value) : : "memory" );
    pmcr_value |= 0x1 << PMC_CNTR_RESET_BIT;

    __asm__ __volatile__(
                        "MSR    PMCR_EL0,   %0"
                        : : "r" (pmcr_value) : "memory" );
}

static __attribute__((always_inline)) void reset_pmc_events() {
    register uint64_t pmcr_value;

    __asm__ __volatile__(
                        "MRS    %0,     PMCR_EL0"
                        : "=r" (pmcr_value) : : "memory" );
    pmcr_value |= 0x1 << PMC_EVENT_RESET_BIT;

    __asm__ __volatile__(
                        "MSR    PMCR_EL0,   %0"
                        : : "r" (pmcr_value) : "memory" );
}

static __attribute__((always_inline)) uint64_t read_pmevcntr(register uint8_t n) {
    register uint64_t pmselr_value;
    register uint64_t pmevcntr_value;

    
    __asm__ __volatile__(
                        "MRS    %0,     PMSELR_EL0"
                        : "=r" (pmselr_value) : : "memory");
    pmselr_value &= ~(PMSELR_MASK);
    pmselr_value |= (n & PMSELR_MASK);

    __asm__ __volatile__(
                        "MSR    PMSELR_EL0,     %0"
                        : : "r" (pmselr_value) : "memory");

    __asm__ __volatile__(
                        "MRS    %0,     PMXEVCNTR_EL0"
                        : "=r" (pmevcntr_value) : : "memory");

    return pmevcntr_value;
}

/*****************FUNCTION DECLARATIONS**************/
void enable_pmc();
void disable_pmc();
void reset_pmc_event(uint8_t n);
void enable_pmevcntr(uint8_t n);
void disable_pmevcntr(uint8_t n);
void cfg_pmevcntr(uint8_t n, uint16_t eventType);
uint64_t read_pmevcntr(uint8_t n);
void initPMU();

void no_allocate_threshold_L1(uint32_t mode);
void no_allocate_threshold_L2(uint32_t mode);

#endif
