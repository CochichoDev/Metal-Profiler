#include <stdint.h>
#include <stdio.h>

#include "PMU.h"
#include "cache_controller.h"
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
#if !defined(SIZE)
#define SIZE 4*128*64
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
extern uint8_t __buffer_start;

int main(int argc, char *argv[]) {
    register volatile uint8_t *target = &__buffer_start;

    // Biggest resolution before unstable
    write_timestampref_div(0x02u);
    enable_cntc();

    set_outstanding_prefetching(0x00U);
    no_allocate_threshold_L1(0b11);
    no_allocate_threshold_L2(0b11);

#ifdef MEMBANDWIDTH
    initPMU();
    init_irq();

    time_handler(PERIOD);

    reset_pmc_events();
    enable_irq();
#endif

    for (HEADER) {
         for (register int i = 0 ; i < SIZE ; i += STRIDE) {
            ACCESS_METHOD(target+i);
        }
    }

#ifdef MEMBANDWIDTH
    disable_cntp();
    stop_irq();
#endif
}

