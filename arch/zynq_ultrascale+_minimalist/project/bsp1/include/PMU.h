#ifndef PMU_H
#define PMU_H

#include "custom_printf.h"

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

/*****************FUNCTION DECLARATIONS**************/
void enable_pmc();
void disable_pmc();
void reset_pmc_event(uint8_t n);
void enable_pmevcntr(uint8_t n);
void disable_pmevcntr(uint8_t n);
void cfg_pmevcntr(uint8_t n, uint16_t eventType);
void initPMU();

void no_allocate_threshold_L1(uint32_t mode);
void no_allocate_threshold_L2(uint32_t mode);

inline uint64_t read_l1d_refills();
inline uint64_t read_l1d_wb();
inline uint64_t read_l2_refills();
inline uint64_t read_l2_wb();

inline void write_l1d_refills(uint64_t value);
inline void write_l1d_wb(uint64_t value);
inline void write_l2_refills(uint64_t value);
inline void write_l2_wb(uint64_t value);

void update_pmc_ref();
void update_pmc_counters();

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

static __attribute__((always_inline)) void write_pmevcntr(register uint8_t n, register uint32_t value) {
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
                        "MRS    %0,  PMXEVCNTR_EL0"
                        : "=r" (pmevcntr_value) : : "memory");

    pmevcntr_value &= (0xFFFFFFFF00000000);
    pmevcntr_value |= value;

    __asm__ __volatile__(
                        "MSR    PMXEVCNTR_EL0,  %0"
                        : : "r" (pmevcntr_value) : "memory");
}

static __attribute__((always_inline)) void set_pmevcntr_threshold(register uint8_t n, register uint32_t value) {
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
                        "MRS    %0,  PMXEVCNTR_EL0"
                        : "=r" (pmevcntr_value) : : "memory");

    pmevcntr_value &= (0xFFFFFFFF00000000);
    pmevcntr_value |= ((~(0x0U)) - value);

    __asm__ __volatile__(
                        "MSR    PMXEVCNTR_EL0,  %0"
                        : : "r" (pmevcntr_value) : "memory");
}

static __attribute__((always_inline)) void clean_overflow_status(register uint8_t n) {
    __asm__ __volatile__(
                        "MSR    PMOVSCLR_EL0,     %0"
                        : : "r" (0x1U << n) : "memory");
}

static __attribute__((always_inline)) void set_overflow_status(register uint8_t n) {
    __asm__ __volatile__(
                        "MSR    PMOVSSET_EL0,     %0"
                        : : "r" (0x1U << n) : "memory");
}

static __attribute__((always_inline)) uint8_t read_overflow_status(register uint8_t n) {
    register uint64_t pmovsclr_value;

    
    __asm__ __volatile__(
                        "MRS    %0,     PMOVSCLR_EL0"
                        : "=r" (pmovsclr_value) : : "memory");
    pmovsclr_value &= (0x1U << n);

    return (pmovsclr_value > 0);
}

static __attribute__((always_inline)) void clean_pmc_int(register uint8_t n) {
    __asm__ __volatile__(
                        "MSR    PMINTENCLR_EL1,     %0"
                        : : "r" (0x1U << n) : "memory");
}

static __attribute__((always_inline)) void set_pmc_int(register uint8_t n) {
    __asm__ __volatile__(
                        "MSR    PMINTENSET_EL1,     %0"
                        : : "r" (0x1U << n) : "memory");
}

static __attribute__((always_inline)) uint8_t read_pmc_int(register uint8_t n) {
    uint64_t pmintenset_value;
    __asm__ __volatile__(
                        "MRS    %0,     PMINTENSET_EL1"
                        : "=r" (pmintenset_value) : : "memory");
    pmintenset_value &= (0x1U << n);
    return (pmintenset_value > 0);
}

#endif
