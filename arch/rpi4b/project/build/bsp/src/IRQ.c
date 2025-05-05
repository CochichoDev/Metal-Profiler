#include "PMU.h"
#include "IRQ.h"
#include "GIC.h"
#include "timer.h"
#include "uart.h"

#if defined(BUDGET) && defined(THRESHOLD)
#define IMP_BL
#endif

//#define DEBUG



#ifdef IMP_BL
static s64 L1D_MSHR_NEW = 0;
static s64 L1D_WB_NEW = 0;
static s64 LAST_L1D_MSHR = 0;
static s64 LAST_L1D_WB = 0;

static u8 COUNTER_RST = 0;

static s64 AVAIL = BUDGET;

static void mem_monitor() {
    LAST_L1D_MSHR = read_pmevcntr(0);
    LAST_L1D_WB = read_pmevcntr(1);

#ifdef DEBUG
    uart_str("AVAIL: "); uart_int(AVAIL); uart_nl();
#endif

    AVAIL = BUDGET;
    
    COUNTER_RST = 1;
}

static void mem_overflow() {
    L1D_MSHR_NEW = read_pmevcntr(0);
    L1D_WB_NEW = read_pmevcntr(1);

    #ifdef DEBUG
        uart_str("Before AVAIL: "); uart_int(AVAIL); uart_nl();
    #endif

    AVAIL-=(L1D_MSHR_NEW - LAST_L1D_MSHR);
    AVAIL-=(L1D_WB_NEW - LAST_L1D_WB);

    #ifdef DEBUG
        uart_str("DELTA_L1D_MSHR: "); uart_int(L1D_MSHR_NEW - LAST_L1D_MSHR); uart_nl();
        uart_str("DELTA_L1D_WB: "); uart_int(L1D_WB_NEW - LAST_L1D_WB); uart_nl();
    #endif
    #ifdef DEBUG
        uart_str("After AVAIL: "); uart_int(AVAIL); uart_nl();
    #endif

    LAST_L1D_MSHR = L1D_MSHR_NEW;
    LAST_L1D_WB = L1D_WB_NEW;

    if (AVAIL <= 0) {
    #ifdef DEBUG
        uart_str("IDLE OVERFLOW"); uart_nl();
    #endif
        asm volatile("wfi");
    }

    clean_overflow_status(4U);
    set_pmevcntr_threshold(4U, BUDGET * THRESHOLD);
}
#endif

void irq_handler() {
    // Read the irq ID
    for (uint32_t iar_value = *REG_GIC_GICC_IAR ; (iar_value & GICC_IAR_INTR_IDMASK) != GICC_IAR_SPURIOUS_INTR ; \
            iar_value = *REG_GIC_GICC_IAR) {

        switch (iar_value & 0x1FFU) {
            case PTIMER_ID:
                *REG_GIC_GICC_EOIR = iar_value;
        #ifdef DEBUG
                uart_str("MemMonitoring Period!"); uart_nl();
        #endif
                disable_cntp();
        #ifdef IMP_BL
                mem_monitor();
        #endif
        #ifdef DEBUG
                uart_str("Leaving timer irq!"); uart_nl();
        #endif
                break;
            case PM1_ID:
            case PM2_ID:
            case PM3_ID:
            case PM4_ID:
                *REG_GIC_GICC_EOIR = iar_value;
        #ifdef DEBUG
                uart_str("Memory limit depassed!"); uart_nl();
        #endif
        #ifdef IMP_BL
                mem_overflow();
        #endif
        #ifdef DEBUG
                uart_str("Leaving overflow irq!"); uart_nl();
        #endif
                break;
            default:
                uart_str("The IRQ ID is "); uart_hex(iar_value & 0x1FFU); uart_nl();
                *REG_GIC_GICC_EOIR = iar_value;
                break;
        }

    }
#ifdef DEBUG
    uart_str("Leaving irq handler!"); uart_nl();
#endif
#ifdef IMP_BL
    if (COUNTER_RST) {
        COUNTER_RST = 0;
        time_handler(PERIOD);
    }
#endif
}

void sync_handler() {
    uart_str("Synchronous Exception"); uart_nl();
}
