#include "PMU.h"
#include <stdint.h>

void initPMU() {
    enable_pmc();
    cfg_pmevcntr(0u, L1D_CACHE_REFILL);
    cfg_pmevcntr(1u, L1D_CACHE_WB);
    cfg_pmevcntr(2u, L2_CACHE_REFILL);
    cfg_pmevcntr(3u, L2_CACHE_WB);
    enable_pmevcntr(0u);
    enable_pmevcntr(1u);
    enable_pmevcntr(2u);
    enable_pmevcntr(3u);
    reset_pmc_events();
}

void reset_pmc_event(uint8_t n) {
    reset_pmc_events();
    /*
    __asm__ __volatile__(
                        "MSR    PMZR_EL0,   %0"
                        : : "r" (0x1 << n) : "memory" );
    */
}

void enable_pmc() {
    register uint64_t pmcr_value;


    __asm__ __volatile__(
                        "MRS    %0,     PMCR_EL0"
                        : "=r" (pmcr_value) : : "memory" );

    pmcr_value |= 0x1 << PMC_ENABLE_BIT;

    __asm__ __volatile__(
                        "MSR    PMCR_EL0,   %0"
                        : : "r" (pmcr_value) : "memory" );
}

void disable_pmc() {
    register uint64_t pmcr_value;

    __asm__ __volatile__(
                        "MRS    %0,     PMCR_EL0"
                        : "=r" (pmcr_value) : : "memory" );

    pmcr_value &= ~(0x1 << PMC_ENABLE_BIT);

    __asm__ __volatile__(
                        "MSR    PMCR_EL0,   %0"
                        : : "r" (pmcr_value) : "memory" );
}

void enable_pmevcntr(uint8_t n) {
    uint64_t pmcntenset_value;

    __asm__ __volatile__(
                        "MRS    %0,     PMCNTENSET_EL0"
                        : "=r" (pmcntenset_value) : : "memory");
    pmcntenset_value |= (0x1U << n);
    __asm__ __volatile__(
                        "MSR    PMCNTENSET_EL0,     %0"
                        : : "r" (pmcntenset_value) : "memory");
}

void disable_pmevcntr(uint8_t n) {
    uint64_t pmcntenclr_value;

    __asm__ __volatile__(
                        "MRS    %0,     PMCNTENCLR_EL0"
                        : "=r" (pmcntenclr_value) : : "memory");
    pmcntenclr_value |= (0x1U << n);
    __asm__ __volatile__(
                        "MSR    PMCNTENCLR_EL0,     %0"
                        : : "r" (pmcntenclr_value) : "memory");
}

void cfg_pmevcntr(uint8_t n, uint16_t eventType) {
    uint64_t pmselr_value;
    uint64_t pmxevtyper_value;

    
    __asm__ __volatile__(
                        "MRS    %0,     PMSELR_EL0"
                        : "=r" (pmselr_value) : : "memory");
    pmselr_value &= ~(PMSELR_MASK);
    pmselr_value |= (n & PMSELR_MASK);

    __asm__ __volatile__(
                        "MSR    PMSELR_EL0,     %0"
                        : : "r" (pmselr_value) : "memory");
    __asm__ __volatile__(
                        "MRS    %0,     PMXEVTYPER_EL0"
                        : "=r" (pmxevtyper_value) : : "memory");

    pmxevtyper_value &= ~PMEVTYPER_MASK;
    pmxevtyper_value |= (eventType & PMEVTYPER_MASK);

    __asm__ __volatile__(
                        "MSR    PMXEVTYPER_EL0, %0"
                        : : "r" (pmxevtyper_value) : "memory");
}

