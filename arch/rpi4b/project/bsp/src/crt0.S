#include "bspconfig.h"

.globl	_startup
_startup:
    ldr x0, =__sbss_start
    ldr x1, =__sbss_end
    cmp x1, x0
    beq sbss_end
sbss_loop:
    str xzr, [x0], #64
    cmp x0, x1
    bne sbss_loop
    
sbss_end:
    ldr x0, =__bss_start
    ldr x1, =__bss_end
    cmp x1, x0
    beq bss_end
bss_loop:
    str xzr, [x0], #64
    cmp x0, x1
    bne bss_loop
    
bss_end:
    /* Prepare stack pointer */
    ldr x0, =__stack_end
    mov sp, x0

	/* Make sure argc and argv are valid */
	mov	x0, #0
	mov	x1, #0

	bl	boot2			/* Jump to second bootloader code */

	bl	exit

