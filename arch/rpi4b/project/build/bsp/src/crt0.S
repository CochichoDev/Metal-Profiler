#include "bspconfig.h"


.globl	_startup
_startup:
    ldr x0, =__bss_start
    ldr x1, =__bss_end
    cmp x1, x0
    beq bss_end
bss_loop:
    str xzr, [x0], #8
    cmp x0, x1
    bne bss_loop
    
bss_end:
    /* Prepare stack pointer */
    /*
    ldr x0, =__stack_end
    mov sp, x0
    */

	/* Make sure argc and argv are valid */
	mov	x0, #0
	mov	x1, #0

    str lr, [sp, #-16]!
	bl	boot2			/* Jump to second bootloader code */
    ldr lr, [sp], #16
    ret

	bl	exit

