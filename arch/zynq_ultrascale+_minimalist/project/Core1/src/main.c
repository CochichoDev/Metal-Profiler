#include <stdint.h>
#include <stdio.h>

#include "PMU.h"
#include "timer.h"
#include "GIC.h"

/*
 *  DUMMY DEFINITIONS FOR WARNING AVOIDANCE
 */
#if !defined(ACCESS_METHOD)
#define ACCESS_METHOD(TARGET)
#endif
#if !defined(HEADER)
#define HEADER ; ; 
#endif
#if !defined(TARGET_SIZE)
#define TARGET_SIZE 4*128*64
#endif
#if !defined(STRIDE)
#define STRIDE 64
#endif

#if !defined(INIT_BUDGET)
#define INIT_BUDGET 1e4
#endif
#if !defined(PERIOD)
#define PERIOD 10
#endif
#if !defined(REPLENISHMENT)
#define REPLENISHMENT 3e4
#endif

#define ENEMY

#include "definitions.h"


extern char __text_start, __text_end;
extern uint8_t __buf_start_1;
extern uint8_t __buf_start_0;

static void mem_monitor() {
    static int64_t avail = INIT_BUDGET;
    static const int64_t threshold = 0;

    static int64_t last_l2_mshr = 0;
    static int64_t last_l2_wb = 0;

    register int64_t l2_mshr_new = read_pmevcntr(0);
    register int64_t l2_wb_new = read_pmevcntr(1);

    avail-=(l2_mshr_new - last_l2_mshr);
    avail-=(l2_wb_new - last_l2_wb);
    avail+=REPLENISHMENT;
    
    last_l2_mshr = l2_mshr_new;
    last_l2_wb = l2_wb_new;

    //printf("#0 -> L2_MSHR: %lld\n", l2_mshr);

    //printf("IDLE %d\n", IDLE);

    while (avail < threshold) {
        time_handler(PERIOD);
        __asm__ __volatile__("wfi");
        disable_cntp();
        avail+=REPLENISHMENT;
    }

    time_handler(PERIOD);
}

void irq_handler() {
    // Read the irq ID
    uint32_t itrID = *REG_GIC_GICC_IAR;

    switch (itrID & 0x1FFU) {
        case PTIMER_ID:
            *REG_GIC_GICC_EOIR = itrID;
            disable_cntp();
            mem_monitor();
            break;
        default:
            printf("The IRQ ID is %d\n", itrID);
            break;
    }

    *REG_GIC_GICC_EOIR = itrID;
}

__attribute__((section(".bench"))) int main(int argc, char *argv[]) {
    register volatile uint8_t *target = &__buf_start_1;

    no_allocate_threshold_L1(0b11);
    no_allocate_threshold_L2(0b11);

#ifdef CACHECOLORING
    //Xil_SetTlbAttributesRange(&__text_start, &__text_end, NORM_NONCACHE);
#endif

#ifdef MEMBANDWIDTH
    initPMU();
    init_irq();

    time_handler(PERIOD);
    enable_irq();

    reset_pmc_events();
#endif

    for (HEADER) {
         for (register int i = 0 ; i < TARGET_SIZE ; i += STRIDE) {
            ACCESS_METHOD(target+i);
        }
    }

#ifdef MEMBANDWIDTH
    disable_cntp();
    stop_irq();
#endif
}

