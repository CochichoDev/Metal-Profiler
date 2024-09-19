#include <stdint.h>
#include <stdio.h>

#include "PMU.h"
#include "GIC.h"
#include "timer.h"
#include "cache_controller.h"

/*
 *  DUMMY DEFINITIONS FOR WARNING AVOIDANCE
 */
#if !defined(ACCESS_METHOD)
#define ACCESS_METHOD(TARGET)
#endif
#if !defined(SIZE)
#define SIZE 4*128*64
#endif
#if !defined(STRIDE)
#define STRIDE 64
#endif
#if !defined(LIMIT)
#define LIMIT 50
#endif

#define VICTIM

#include "definitions.h"

extern char __text_start;
extern char __text_end;
extern uint8_t __buffer_start;

/* SPECIFIC ARMV8-A CODE */
/* ONLY TO GUARANTEE REGISTER USE (NO ADDITIONAL MEM REQUESTS) */
register uint64_t SC_TICKS __asm__("x28");
register uint64_t L1D_REFILLS __asm__("x27");
register uint64_t L1D_WB __asm__("x26");
register uint64_t L2_REFILLS __asm__("x25");
register uint64_t L2_WB __asm__("x24");


int main(int argc, char *argv[]) {
    set_outstanding_prefetching(0x00U);
    register volatile uint8_t *target = &__buffer_start;

    // Biggest resolution before unstable
    write_timestampref_div(0x02u);
    enable_cntc();

    initPMU();
#ifdef MEMBANDWIDTH
    init_irq();

    time_handler(PERIOD);

    reset_pmc_events();
    enable_irq();
#endif


    //INIT();
    /*
     * This initialization of SC_TICKS is needed since from it is intended to
     * minimize the time between printing the value of the system counter ticks
     * and updating the SC_TICKS later in the loop. This is fundamental since
     * in order to not disregard te cycles that the MemMonitoring application
     * adds, one need to disable the interrupts until the information of the
     * cycles is not recovered.
     */
    SC_TICKS    = read_cntpct_el0();
    L1D_REFILLS = read_pmevcntr(0);
    L1D_WB      = read_pmevcntr(1);
    L2_REFILLS  = read_pmevcntr(2);
    L2_WB       = read_pmevcntr(3);
    for (HEADER) 
    {
        for (register int i = 0 ; i < SIZE ; i += STRIDE) 
        {
            ACCESS_METHOD(target+i);
        }
    #ifdef MEMBANDWIDTH
        disable_irq();
    #endif
        printf("R%lu\n", read_cntpct_el0() - SC_TICKS);
        printf("R%lu\n", read_pmevcntr(0) - L1D_REFILLS);
        printf("R%lu\n", read_pmevcntr(1) - L1D_WB);
        printf("R%lu\n", read_pmevcntr(2) - L2_REFILLS);
        printf("R%lu\n", read_pmevcntr(3) - L2_WB);
        L1D_REFILLS = read_pmevcntr(0);
        L1D_WB      = read_pmevcntr(1);
        L2_REFILLS  = read_pmevcntr(2);
        L2_WB       = read_pmevcntr(3);
        SC_TICKS    = read_cntpct_el0();
    #ifdef MEMBANDWIDTH
        enable_irq();
    #endif
    }
    printf("FIN\n");

#ifdef MEMBANDWIDTH
    disable_cntp();
    stop_irq();
#endif
}

