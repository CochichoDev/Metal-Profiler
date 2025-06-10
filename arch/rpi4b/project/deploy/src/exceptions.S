#define ASSEMBLY

#include "gic.h"

.section .text

.globl _sync_exception
.globl _irq_exception
.globl _fiq_exception
.globl _serror_exception


_irq_exception:
    b irq_handler
_sync_exception:
    b sync_handler
_fiq_exception:
_serror_exception:
    /* Make LED blink forever */
    ldr x0, =0xfe200000
    ldr w1, [x0]
    ldr w2, =0xffff8fff
    and w1, w1, w2
    orr w1, w1, #0x1000
    str w1, [x0]

loop:
    ldr x0, =0xfe20001c
    orr w1, wzr, #0x10
    str w1, [x0]

    ldr x0, =5000000
    bl wait

    ldr x0, =0xfe200028
    orr w1, wzr, #0x10
    str w1, [x0]

    ldr x0, =5000000
    bl wait

    b loop
