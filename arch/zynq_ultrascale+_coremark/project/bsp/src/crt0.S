#include "xparameters.h"
#include "bspconfig.h"

	.text
.Lsbss_start:
	.quad	__sbss_start

.Lsbss_end:
	.quad	__sbss_end

.Lbss_start:
	.quad	__bss_start

.Lbss_end:
	.quad	__bss_end


.globl	_startup
_startup:

	/* clear sbss */
    eor x0, x0, x0
	ldr 	x1,.Lsbss_start		/* calculate beginning of the SBSS */
	ldr	x2,.Lsbss_end		/* calculate end of the SBSS */

.Lloop_sbss:
	cmp	x1,x2
	bge	.Lenclsbss		/* If no SBSS, no clearing required */
	str	x0, [x1], #8
	b	.Lloop_sbss

.Lenclsbss:
	/* clear bss */
	ldr	x1,.Lbss_start		/* calculate beginning of the BSS */
	ldr	x2,.Lbss_end		/* calculate end of the BSS */

.Lloop_bss:
	cmp	x1,x2
	bge	.Lenclbss		/* If no BSS, no clearing required */
	str	x0, [x1], #8
	b	.Lloop_bss

.Lenclbss:
	/* Reset and start Triple Timer Counter */

	/* make sure argc and argv are valid */
	mov	x0, #0
	mov	x1, #0

	bl	boot2			/* Jump to second bootloader code */

	bl	exit

.Lexit:	/* should never get here */
	b .Lexit

.Lstart:
	.size	_startup,.Lstart-_startup
