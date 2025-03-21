#ifndef PMU_H
#define PMU_H

#include "common.h"

#define PMC_CNTR_RESET_BIT      (2)
#define PMC_EVENT_RESET_BIT     (1)
#define PMC_ENABLE_BIT          (0)

#define PMCNTENSET_CNTR_BIT     (31)

#define PMCNTENCLR_CNTR_BIT     (31)

#define PMSELR_MASK             (0x1FU)

#define PMEVTYPER_MASK          (0xFFFFU)

/****************EVENTS*****************/
#define SW_INCR             (0x00U)
#define L1D_CACHE_REFILL    (0x03U)
#define CPU_CYCLES          (0x11U)
#define BR_PRED             (0x12U)
#define L1I_CACHE           (0x14U)
#define L1D_CACHE_WB        (0x15U)
#define L2_CACHE_REFILL     (0x17U)
#define L2_CACHE_WB         (0x18U)
#define BUS_CYCLES          (0x1DU)

/*****************FUNCTION DECLARATIONS**************/
void enable_pmc();
void disable_pmc();
void reset_pmc_event(u8 n);
void enable_pmevcntr(u8 n);
void enable_pmccntr();
void disable_pmevcntr(u8 n);
void cfg_pmevcntr(u8 n, uint16_t eventType);
void initPMU();

void no_allocate_threshold_L1(u32 mode);
void no_allocate_threshold_L2(u32 mode);

inline u64 read_l1d_refills();
inline u64 read_l1d_wb();
inline u64 read_l2_refills();
inline u64 read_l2_wb();

inline void write_l1d_refills(u64 value);
inline void write_l1d_wb(u64 value);
inline void write_l2_refills(u64 value);
inline void write_l2_wb(u64 value);

void update_pmc_ref();
void update_pmc_counters();

/*****************INLINE FUNCTION DEFINITION**************/
static __attribute__((always_inline)) void reset_pmccntr() {
    register u64 pmcr_value;

    __asm__ __volatile__(
                        "MRS    %0,     PMCR_EL0"
                        : "=r" (pmcr_value) : : "memory" );
    pmcr_value |= 0x1 << PMC_CNTR_RESET_BIT;

    __asm__ __volatile__(
                        "MSR    PMCR_EL0,   %0"
                        : : "r" (pmcr_value) : "memory" );
}

static __attribute__((always_inline)) void reset_pmc_events() {
    register u64 pmcr_value;

    __asm__ __volatile__(
                        "MRS    %0,     PMCR_EL0"
                        : "=r" (pmcr_value) : : "memory" );
    pmcr_value |= 0x1 << PMC_EVENT_RESET_BIT;

    __asm__ __volatile__(
                        "MSR    PMCR_EL0,   %0"
                        : : "r" (pmcr_value) : "memory" );
}

static __attribute__((always_inline)) u64 read_pmevcntr_0() {
    register u64 pmevcntr_value;

    __asm__ __volatile__(
                        "MRS    %0,     PMEVCNTR0_EL0"
                        : "=r" (pmevcntr_value) : : "memory");

    return pmevcntr_value;
}

static __attribute__((always_inline)) u64 read_pmevcntr(register u8 n) {
    register u64 pmselr_value;
    register u64 pmevcntr_value;

    
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

static __attribute__((always_inline)) u64 read_pmccntr() {
    register u64 pmccntr_value;

    
    __asm__ __volatile__(
                        "MRS    %0,     PMCCNTR_EL0"
                        : "=r" (pmccntr_value) : : "memory");

    return pmccntr_value;
}

static __attribute__((always_inline)) void write_pmevcntr(register u8 n, register u32 value) {
    register u64 pmselr_value;
    register u64 pmevcntr_value;
    
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

static __attribute__((always_inline)) void set_pmevcntr_threshold(register u8 n, register u32 value) {
    register u64 pmselr_value;
    register u64 pmevcntr_value;
    
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

static __attribute__((always_inline)) void clean_overflow_status(register u8 n) {
    __asm__ __volatile__(
                        "MSR    PMOVSCLR_EL0,     %0"
                        : : "r" (0x1U << n) : "memory");
}

static __attribute__((always_inline)) void set_overflow_status(register u8 n) {
    __asm__ __volatile__(
                        "MSR    PMOVSSET_EL0,     %0"
                        : : "r" (0x1U << n) : "memory");
}

static __attribute__((always_inline)) u8 read_overflow_status(register u8 n) {
    register u64 pmovsclr_value;

    
    __asm__ __volatile__(
                        "MRS    %0,     PMOVSCLR_EL0"
                        : "=r" (pmovsclr_value) : : "memory");
    pmovsclr_value &= (0x1U << n);

    return (pmovsclr_value > 0);
}

static __attribute__((always_inline)) void clean_pmc_int(register u8 n) {
    __asm__ __volatile__(
                        "MSR    PMINTENCLR_EL1,     %0"
                        : : "r" (0x1U << n) : "memory");
}

static __attribute__((always_inline)) void set_pmc_int(register u8 n) {
    __asm__ __volatile__(
                        "MSR    PMINTENSET_EL1,     %0"
                        : : "r" (0x1U << n) : "memory");
}

static __attribute__((always_inline)) u8 read_pmc_int(register u8 n) {
    u64 pmintenset_value;
    __asm__ __volatile__(
                        "MRS    %0,     PMINTENSET_EL1"
                        : "=r" (pmintenset_value) : : "memory");
    pmintenset_value &= (0x1U << n);
    return (pmintenset_value > 0);
}

#endif
