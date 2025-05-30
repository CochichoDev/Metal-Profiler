#include "cache_controller.h"
#include "gpio.h"
#include "uart.h"
#include "definitions.h"
#include "PMU.h"
#include "timer.h"

extern u8 __buffer_start;

#ifdef VICTIM
register uint64_t SC_TICKS __asm__("x28");
register uint64_t L1D_REFILLS __asm__("x27");
register uint64_t L1D_WB __asm__("x26");
register uint64_t L2_REFILLS __asm__("x25");
register uint64_t L2_WB __asm__("x24");
#endif

int main(void) {
    disable_no_allocate();
    disable_va_based_prefetching();
    disable_outstanding_prefetching();
    register volatile uint8_t *target = &__buffer_start;

#ifdef VICTIM
    SC_TICKS    = read_cntpct_el0();
    L1D_REFILLS = read_pmevcntr(0);
    L1D_WB      = read_pmevcntr(1);
    L2_REFILLS  = read_pmevcntr(2);
    L2_WB       = read_pmevcntr(3);
#endif
    for (HEADER) 
    {
        for (register int j = 0 ; j < DDR/SIZE ; ++j)
        {
            for (register int i = 0 ; i < (SIZE * DIV) ; i += STRIDE) 
            {
                ACCESS_METHOD(target+i);
            }
        }
#ifdef VICTIM
        uart_str("!"); uart_int(read_cntpct_el0() - SC_TICKS); uart_nl();
        uart_str("!"); uart_int(read_pmevcntr(0) - L1D_REFILLS); uart_nl();
        uart_str("!"); uart_int(read_pmevcntr(1) - L1D_WB); uart_nl();
        uart_str("!"); uart_int(read_pmevcntr(2) - L2_REFILLS); uart_nl();
        uart_str("!"); uart_int(read_pmevcntr(3) - L2_WB); uart_nl();
        asm volatile("dsb sy");
        L1D_REFILLS = read_pmevcntr(0);
        L1D_WB      = read_pmevcntr(1);
        L2_REFILLS  = read_pmevcntr(2);
        L2_WB       = read_pmevcntr(3);
        SC_TICKS    = read_cntpct_el0();
#endif
    }
#ifdef VICTIM
    uart_str("?"); uart_nl();
#endif

    return 0;
}
