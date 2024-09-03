#include <stdint.h>
#include <stdio.h>

#include "PMU.h"
#include "IRQ.h"
#include "GIC.h"
#include "timer.h"

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
