#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define CNTV_CTL_ENABLE     (0x1U)
#define CNTV_CTL_IMASK      (0x2U)

#define CNTP_CTL_ENABLE     (0x1U)
#define CNTP_CTL_IMASK      (0x2U)

#define CNT_FREQ_MASK       (0xFFFFFFFFU)

#define CNTControlBase      (0xFF260000)
#define CNTCR               (CNTControlBase + 0x000U)
#define CNTSR               (CNTControlBase + 0x004U)
#define CNTCV               (CNTControlBase + 0x008U)
#define CNTID               (CNTControlBase + 0x01CU)
#define CNTFID0             (CNTControlBase + 0x020U)

#define REG_CNTCR           ((volatile uint32_t *)(uintptr_t)CNTCR)
#define REG_CNTSR           ((volatile uint32_t *)(uintptr_t)CNTSR)
#define REG_CNTCV           ((volatile uint64_t *)(uintptr_t)CNTCV)
#define REG_CNTID           ((volatile uint32_t *)(uintptr_t)CNTID)
#define REG_CNTFID0         ((volatile uint64_t *)(uintptr_t)CNTFID0)

#define CRL_APB_TIMESTAMP_REF_CTRL_OFFSET   (0xFF5E0128U)

#define CNTCR_MASK          (0xFFFFFFF4U)
#define CNTCR_SCALING_MASK  (0xFFFFFFF8U)


#define IOPLLFRQ            (1500000000)

/*********** VIRTUAL TIMER ************/
static __attribute__((always_inline)) void write_cntv_tval_el0(uint64_t cntv_tval_value) {
    __asm__ __volatile__(
                            "MSR    CNTV_TVAL_EL0,      %0" 
                        : : "r" (cntv_tval_value) : "memory" );
}

static __attribute__((always_inline)) void disable_cntv() {
    uint64_t cntv_ctl;
    __asm__ __volatile__  (
                            "MRS     %0,     CNTV_CTL_EL0\n\t"
                          : "=r" (cntv_ctl) :: "memory");
    cntv_ctl &= ~CNTV_CTL_ENABLE;
    cntv_ctl |= CNTV_CTL_IMASK;
    __asm__ __volatile__  (
                            "MSR     CNTV_CTL_EL0,      %0\n\t"
                          : : "r" (cntv_ctl) : "memory");
}

static __attribute__((always_inline)) void enable_cntv() {
    register uint64_t cntv_ctl;
    __asm__ __volatile__  (
                            "MRS     %0,     CNTV_CTL_EL0\n\t"
                          : "=r" (cntv_ctl) :: "memory");
    cntv_ctl |= CNTV_CTL_ENABLE;
    cntv_ctl &= ~CNTV_CTL_IMASK;
    __asm__ __volatile__  (
                            "MSR     CNTV_CTL_EL0,      %0\n\t"
                          : : "r" (cntv_ctl) : "memory");
}

static __attribute__((always_inline)) void write_cntv_cval_el0(uint64_t cntv_cval_value) {
    __asm__ __volatile__(
                            "MSR    CNTV_CVAL_EL0,      %0" 
                        : : "r" (cntv_cval_value) : "memory" );
}

static __attribute__((always_inline)) uint64_t read_cntvct_el0() {
    uint64_t cntvct_value;

    __asm__ __volatile__(
                            "MRS    %0,     CNTVCT_EL0" 
                        : "=r" (cntvct_value) : : "memory" );

    return cntvct_value;
}

static __attribute__((always_inline)) uint32_t read_cntfreq_el0() {
     register uint64_t cntfreq_value;

    __asm__ __volatile__(
                            "MRS    %0,     CNTFRQ_EL0" 
                        : "=r" (cntfreq_value) : : "memory" );

    return (cntfreq_value & CNT_FREQ_MASK);   
}


static __attribute__((always_inline)) void vtime_handler(uint64_t us) {
    disable_cntv();

    // Next time it will interrupt
    // Divided the frequency by 2 cause of the divisor
    write_cntv_tval_el0((int32_t) (((double)us/1000000.0f) * (double)(IOPLLFRQ)/2));

    enable_cntv();
}

/*********** PHYSICAL TIMER ************/
static __attribute__((always_inline)) void write_cntp_tval_el0(uint64_t cntp_tval_value) {
    __asm__ __volatile__(
                            "MSR    CNTP_TVAL_EL0,      %0" 
                        : : "r" (cntp_tval_value) : "memory" );
}

static __attribute__((always_inline)) uint64_t read_cntp_tval_el0() {
    uint64_t cntp_tval_value;
    __asm__ __volatile__(
                            "MRS    %0,      CNTP_TVAL_EL0" 
                        : "=r" (cntp_tval_value) : : "memory" );
    return cntp_tval_value;
}

static __attribute__((always_inline)) void disable_cntp() {
    register uint64_t cntp_ctl;
    __asm__ __volatile__  (
                            "MRS     %0,     CNTP_CTL_EL0\n\t"
                          : "=r" (cntp_ctl) :: "memory");
    cntp_ctl &= ~CNTV_CTL_ENABLE;
    cntp_ctl |= CNTV_CTL_IMASK;
    __asm__ __volatile__  (
                            "MSR     CNTP_CTL_EL0,      %0\n\t"
                          : : "r" (cntp_ctl) : "memory");
}

static __attribute__((always_inline)) void enable_cntp() {
    register uint64_t cntp_ctl;
    __asm__ __volatile__  (
                            "MRS     %0,     CNTP_CTL_EL0\n\t"
                          : "=r" (cntp_ctl) :: "memory");
    cntp_ctl |= CNTP_CTL_ENABLE;
    cntp_ctl &= ~CNTP_CTL_IMASK;
    __asm__ __volatile__  (
                            "MSR     CNTP_CTL_EL0,      %0\n\t"
                          : : "r" (cntp_ctl) : "memory");
}

static __attribute__((always_inline)) void write_cntp_cval_el0(uint64_t cntp_cval_value) {
    __asm__ __volatile__(
                            "MSR    CNTP_CVAL_EL0,      %0" 
                        : : "r" (cntp_cval_value) : "memory" );
}

static __attribute__((always_inline)) uint64_t read_cntpct_el0() {
    uint64_t cntpct_value;

    __asm__ __volatile__(
                            "MRS    %0,     CNTPCT_EL0" 
                        : "=r" (cntpct_value) : : "memory" );

    return cntpct_value;
}

static __attribute__((always_inline)) void time_handler(uint64_t us) {
    disable_cntp();

    // Next time it will interrupt
    // Divided the frequency by 2 cause of the divisor
    write_cntp_tval_el0((int32_t) (((double)us/1000000.0f) * (double)(IOPLLFRQ)/2));

    enable_cntp();
}

/*********** SYSTEM COUNTER ************/
static inline void write_timestampref_div(uint8_t value) {
    uint32_t previous = *((volatile uint32_t *)CRL_APB_TIMESTAMP_REF_CTRL_OFFSET);

    previous &= ~0x3F00U;
    previous |= (value << 8);

    *((volatile uint32_t *)CRL_APB_TIMESTAMP_REF_CTRL_OFFSET) = previous;
}

static inline uint8_t read_cntid() {
    return (*REG_CNTID & 0xFU);
}

static inline void disable_cntc() {
    register uint32_t CNTCR_OG_VALUE = *REG_CNTCR;
    CNTCR_OG_VALUE &= (read_cntid()) ? CNTCR_SCALING_MASK : CNTCR_MASK;
    *REG_CNTCR = CNTCR_OG_VALUE;
}

static inline void enable_cntc() {
    register uint32_t CNTCR_OG_VALUE = *REG_CNTCR;
    CNTCR_OG_VALUE &= (read_cntid()) ? CNTCR_SCALING_MASK : CNTCR_MASK;
    CNTCR_OG_VALUE |= 0x1U;
    *REG_CNTCR = CNTCR_OG_VALUE;
}

static __attribute__((always_inline)) uint64_t read_cntc_value() {
    return *REG_CNTCV;
}

static inline uint32_t read_cntc_freq() {
    return *REG_CNTFID0;
}

#endif
