#include "IRQ.h"
#include "timer.h"
#include "PMU.h"

int main(void);

void boot2() {
    // Biggest resolution before unstable
    //enable_cntc();
    initPMU();
#ifdef BANDLIMIT
    init_irq();
    time_handler(PERIOD);
    reset_pmc_events();
    set_pmc_int(4U);
    set_pmevcntr_threshold(4U, BUDGET * THRESHOLD);
    asm volatile ("msr DAIFClr, 0x2");
#endif
    main();
    //disable_cntp();
#ifdef BANDLIMIT
    stop_irq();
#endif
}
