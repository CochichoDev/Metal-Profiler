#include <stdint.h>
#include <stdio.h>

#include "PMU.h"
#include "GIC.h"
#include "timer.h"

/*
 *  DUMMY DEFINITIONS FOR WARNING AVOIDANCE
 */
#if !defined(ACCESS_METHOD)
#define ACCESS_METHOD(TARGET)
#endif
#if !defined(TARGET_SIZE)
#define TARGET_SIZE 4*128*64
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


__attribute__((section(".bench"))) int main(int argc, char *argv[]) {
    //Xil_ConfigureL1Prefetch(0x0U);
    register uint64_t SC_TICKS = 0;
    register volatile uint8_t *target = &__buffer_start;

    // Biggest resolution before unstable
    write_timestampref_div(0x02u);

    enable_cntc();

    //Xil_SetTlbAttributesRange((UINTPTR) &__text_start, (UINTPTR) &__text_end, NORM_NONCACHE);

    initPMU();
#ifdef MEMBANDWIDTH
    init_irq();

    time_handler(PERIOD);
    enable_irq();

    reset_pmc_events();
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
    SC_TICKS = read_cntpct_el0();
    for (HEADER) 
    {
    #ifndef MEMBANDWIDTH
        reset_pmc_events();
    #endif
        for (register int i = 0 ; i < TARGET_SIZE ; i += STRIDE) 
        {
            ACCESS_METHOD(target+i);
        }
    #ifdef MEMBANDWIDTH
        disable_irq();
    #endif
        printf("%lu\n", read_cntpct_el0() - SC_TICKS);
        printf("%lu\n", read_pmevcntr(0));
        SC_TICKS = read_cntpct_el0();
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

