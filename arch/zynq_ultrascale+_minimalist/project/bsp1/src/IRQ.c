#include <stdint.h>
#include <stdio.h>

#include "PMU.h"
#include "IRQ.h"
#include "GIC.h"
#include "timer.h"

//#define DEBUG

static int64_t L1D_MSHR_NEW = 0;
static int64_t L1D_WB_NEW = 0;
static int64_t LAST_L1D_MSHR = 0;
static int64_t LAST_L1D_WB = 0;

static int64_t AVAIL = BUDGET;

static void mem_blocked() {
    while (AVAIL < 0) {
        vtime_handler(PERIOD);
        __asm__ __volatile__("wfi");
        disable_cntv();
    #ifdef DEBUG
        printf("IDLE\n");
    #endif
        AVAIL+=REPLENISHMENT;
    }
}

static void mem_monitor() {
    L1D_MSHR_NEW = read_pmevcntr(0);
    L1D_WB_NEW = read_pmevcntr(1);

    AVAIL-=(L1D_MSHR_NEW - LAST_L1D_MSHR);
    AVAIL-=(L1D_WB_NEW - LAST_L1D_WB);

    AVAIL+= REPLENISHMENT;
    if (AVAIL > BUDGET) AVAIL = BUDGET;

    LAST_L1D_MSHR = L1D_MSHR_NEW;
    LAST_L1D_WB = L1D_WB_NEW;

    if (AVAIL < 0) mem_blocked();
    
    time_handler(PERIOD);
}

static void mem_overflow() {
    L1D_MSHR_NEW = read_pmevcntr(0);
    L1D_WB_NEW = read_pmevcntr(1);

    AVAIL-=(L1D_MSHR_NEW - LAST_L1D_MSHR);
    AVAIL-=(L1D_WB_NEW - LAST_L1D_WB);

    LAST_L1D_MSHR = L1D_MSHR_NEW;
    LAST_L1D_WB = L1D_WB_NEW;

    if (AVAIL < 0) {
    #ifdef DEBUG
        printf("IDLE OVERFLOW\n");
    #endif
        asm volatile("wfi");
    }

    clean_overflow_status(4U);
    set_pmevcntr_threshold(4U, THRESHOLD);
}

void irq_handler() {
    // Read the irq ID
    for (uint32_t iar_value = *REG_GIC_GICC_IAR ; (iar_value & GICC_IAR_INTR_IDMASK) != GICC_IAR_SPURIOUS_INTR ; \
            iar_value = *REG_GIC_GICC_IAR) {

        switch (iar_value & 0x1FFU) {
            case PTIMER_ID:
                *REG_GIC_GICC_EOIR = iar_value;
        #ifdef DEBUG
                printf("MemMonitoring Period!\n");
        #endif
                disable_cntp();
                mem_monitor();
        #ifdef DEBUG
                printf("Leaving timer irq!\n");
            #endif
                break;
            case PM1_ID:
            case PM2_ID:
            case PM3_ID:
            case PM4_ID:
                *REG_GIC_GICC_EOIR = iar_value;
            #ifdef DEBUG
                printf("Memory limit depassed!\n");
            #endif
                mem_overflow();
            #ifdef DEBUG
                printf("Leaving overflow irq!\n");
            #endif
                break;
            default:
                *REG_GIC_GICC_EOIR = iar_value;
                printf("The IRQ ID is %d\n", iar_value & 0x1FFU);
                break;
        }

    }
#ifdef DEBUG
    printf("Leaving irq handler!\n");
#endif
}

void sync_handler() {
    printf("Synchronous Exception\n");
}
