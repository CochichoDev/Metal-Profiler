#include "IRQ.h"
#include "timer.h"
#include "PMU.h"

void boot2() {
    // Biggest resolution before unstable
    write_timestampref_div(0x02u);
    enable_cntc();
    initPMU();
#ifdef MEMBANDWIDTH
    init_irq();
    time_handler(PERIOD);
    reset_pmc_events();
    set_pmc_int(4U);
    set_pmevcntr_threshold(4U, THRESHOLD);
    asm volatile ("msr DAIFClr, 0x2");
#endif
    asm volatile("bl main");
    disable_cntp();
#ifdef MEMBANDWIDTH
    stop_irq();
#endif
}
