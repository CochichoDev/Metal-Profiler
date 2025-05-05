#include "PMU.h"
#include "uart.h"
#include <stdint.h>

static uint64_t SC_TICKS = 0;
static uint64_t L1D_REFILLS = 0;
static uint64_t L1D_WB = 0;
static uint64_t L2_REFILLS = 0;
static uint64_t L2_WB = 0;

static uint64_t SC_TICKS_REF = 0;
static uint64_t L1D_REFILLS_REF = 0;
static uint64_t L1D_WB_REF = 0;
static uint64_t L2_REFILLS_REF = 0;
static uint64_t L2_WB_REF = 0;

void cfg_pmevcntr_0(uint16_t eventType);
void print_available_events();
void clear_of_flags();

void initPMU() {
    print_available_events();
    enable_pmc();
    clear_of_flags();
    cfg_pmevcntr(0u, L1D_CACHE_REFILL);
    cfg_pmevcntr(1u, L1D_CACHE_WB);
    cfg_pmevcntr(2u, L2_CACHE_REFILL);
    cfg_pmevcntr(3u, L2_CACHE_WB);
    cfg_pmevcntr(4u, L1D_CACHE_REFILL);
    cfg_pmevcntr(5u, L1D_CACHE_WB);
    enable_pmevcntr(0u);
    enable_pmevcntr(1u);
    enable_pmevcntr(2u);
    enable_pmevcntr(3u);
    enable_pmevcntr(4u);
    enable_pmevcntr(5u);
    reset_pmc_events();

    /*
    L1D_REFILLS_REF = read_pmevcntr(0);
    L1D_WB_REF = read_pmevcntr(1);
    L2_REFILLS_REF = read_pmevcntr(2);
    L2_WB_REF = read_pmevcntr(3);
    */
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
    __asm__ __volatile__("isb");

    pmcr_value |= 0x1 << PMC_ENABLE_BIT;
    pmcr_value |= 0x1 << 4;         // Enable export events

    #ifdef DEBUG
    uart_str("PMCR_EL0: "); uart_hex(pmcr_value); uart_nl();
    #endif

    __asm__ __volatile__(
                        "MSR    PMCR_EL0,   %0"
                        : : "r" (pmcr_value) : "memory" );
    __asm__ __volatile__("isb");
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

void clear_of_flags() {
    __asm__ __volatile__(
                        "MSR    PMOVSCLR_EL0,     %0"
                        : : "r" (0xFFFFFFFF) : "memory");
    __asm__ __volatile__("isb");
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
    __asm__ __volatile__("isb");
}

void enable_pmccntr() {
    uint64_t pmcntenset_value;

    __asm__ __volatile__(
                        "MRS    %0,     PMCNTENSET_EL0"
                        : "=r" (pmcntenset_value) : : "memory");
    __asm__ __volatile__("isb");
    pmcntenset_value |= (0x1U << 31);
    __asm__ __volatile__(
                        "MSR    PMCNTENSET_EL0,     %0"
                        : : "r" (pmcntenset_value) : "memory");
    __asm__ __volatile__("isb");

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
    __asm__ __volatile__("isb");
    #ifdef DEBUG
    uart_str("PMSELR_EL0 (Before): "); uart_hex(pmselr_value); uart_nl(); 
    #endif
    pmselr_value &= ~(PMSELR_MASK);
    pmselr_value |= (n & PMSELR_MASK);
    #ifdef DEBUG
    uart_str("PMSELR_EL0 (After): "); uart_hex(pmselr_value); uart_nl(); 
    #endif

    __asm__ __volatile__(
                        "MSR    PMSELR_EL0,     %0"
                        : : "r" (pmselr_value) : "memory");
    __asm__ __volatile__("isb");
    __asm__ __volatile__(
                        "MRS    %0,     PMXEVTYPER_EL0"
                        : "=r" (pmxevtyper_value) : : "memory");
    __asm__ __volatile__("isb");

    pmxevtyper_value &= ~PMEVTYPER_MASK;
    pmxevtyper_value |= (eventType & PMEVTYPER_MASK);

    __asm__ __volatile__(
                        "MSR    PMXEVTYPER_EL0, %0"
                        : : "r" (pmxevtyper_value) : "memory");
    __asm__ __volatile__("isb");
}

void cfg_pmevcntr_0(uint16_t eventType) {
    uint64_t pmevtyper_value;

    
    __asm__ __volatile__(
                        "MRS    %0,     PMEVTYPER0_EL0"
                        : "=r" (pmevtyper_value) : : "memory");
    __asm__ __volatile__("isb");

    pmevtyper_value &= ~PMEVTYPER_MASK;
    pmevtyper_value |= (eventType & PMEVTYPER_MASK);

    #ifdef DEBUG
    uart_str("PMEVTYPER0_EL0: "); uart_hex(pmevtyper_value); uart_nl();
    #endif

    __asm__ __volatile__(
                        "MSR    PMEVTYPER0_EL0, %0"
                        : : "r" (pmevtyper_value) : "memory");
    __asm__ __volatile__("isb");
}

void print_available_events() {
    volatile u64 pmceid_value = 0;

    __asm__ __volatile__(
                        "MRS    %0, PMCEID0_EL0"
                        : "=r" (pmceid_value) : : "memory");
    __asm__ __volatile__("isb");
    #ifdef DEBUG
    uart_str("PMCEID0_EL0: "); uart_hex(pmceid_value); uart_nl();
    #endif
}

inline uint64_t read_l1d_refills() { return L1D_REFILLS; }
inline uint64_t read_l1d_wb() { return L1D_WB; }
inline uint64_t read_l2_refills() { return L2_REFILLS; }
inline uint64_t read_l2_wb() { return L2_WB; }

inline void write_l1d_refills(uint64_t value) { L1D_REFILLS = value; }
inline void write_l1d_wb(uint64_t value) { L1D_WB = value; }
inline void write_l2_refills(uint64_t value) { L2_REFILLS = value; }
inline void write_l2_wb(uint64_t value) { L2_WB = value; }

inline void update_pmc_ref() {
    L1D_REFILLS_REF = read_pmevcntr(0);
    L1D_WB_REF = read_pmevcntr(1);
    L2_REFILLS_REF = read_pmevcntr(2);
    L2_WB_REF = read_pmevcntr(3);
}

inline void update_pmc_counters() {
    write_l1d_refills((read_overflow_status(0)) ? 
                      ( read_pmevcntr(0) + ~(0x0UL) - L1D_REFILLS_REF + L1D_REFILLS ) : 
                      ( read_pmevcntr(0) - L1D_REFILLS_REF + L1D_REFILLS) );
    write_l1d_wb((read_overflow_status(1)) ? 
                      ( read_pmevcntr(1) + ~(0x0UL) - L1D_WB_REF + L1D_WB ) : 
                      ( read_pmevcntr(1) - L1D_WB_REF + L1D_WB) );
    write_l2_refills((read_overflow_status(2)) ? 
                      ( read_pmevcntr(2) + ~(0x0UL) - L2_REFILLS_REF + L2_REFILLS ) : 
                      ( read_pmevcntr(2) - L2_REFILLS_REF + L2_REFILLS) );
    write_l2_wb((read_overflow_status(3)) ? 
                      ( read_pmevcntr(3) + ~(0x0UL) - L2_WB_REF + L2_WB ) : 
                      ( read_pmevcntr(3) - L2_WB_REF + L2_WB) );
}
