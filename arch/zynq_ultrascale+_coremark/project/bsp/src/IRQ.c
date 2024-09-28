#include <stdint.h>
#include <stdio.h>

#include "PMU.h"
#include "IRQ.h"
#include "GIC.h"
#include "timer.h"

static void mem_monitor() {
    static volatile int64_t avail = INIT_BUDGET;
    static volatile const int64_t threshold = 0;

    static int64_t last_l2_mshr = 0;
    static int64_t last_l2_wb = 0;

    volatile register int64_t l2_mshr_new = read_pmevcntr(2);
    volatile register int64_t l2_wb_new = read_pmevcntr(3);

    avail-=(l2_mshr_new - last_l2_mshr);
    avail-=(l2_wb_new - last_l2_wb);
    avail+=REPLENISHMENT;
    
    last_l2_mshr = l2_mshr_new;
    last_l2_wb = l2_wb_new;

    //printf("#0 -> L2_MSHR: %lld\n", l2_mshr);

    while (avail < threshold) {
        time_handler(PERIOD);
        __asm__ __volatile__("wfi");
        disable_cntp();
        //printf("IDLE\n");
        avail+=REPLENISHMENT;
    }

    //gicd_clear_pending(PTIMER_ID); TRY THIS
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
            //*REG_GIC_GICC_EOIR = itrID; TRY THIS
            return;
        default:
            *REG_GIC_GICC_EOIR = itrID;
            printf("The IRQ ID is %d\n", itrID);
            break;
    }
}

void sync_handler() {
    printf("Synchronous Exception\n");
}
