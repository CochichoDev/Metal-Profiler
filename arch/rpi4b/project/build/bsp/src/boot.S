#include "bspconfig.h"

.extern MMUTableL0.0

.global _prestart
.global _boot

.extern __stack_end
.global _vector_table

.set L0Table,	MMUTableL0.0
.set L1Table,	MMUTableL1.0

.section .boot,"ax"

_boot:
	/*Define stack pointer for current exception level*/
	ldr	 x2, =__stack_end
	mov	 sp, x2

    /* Save previous link address in new stack */
    str lr, [sp, #-16]!

	mov      x0, #0
	mov      x1, #0
	mov      x2, #0
	mov      x3, #0
	mov      x4, #0
	mov      x5, #0
	mov      x6, #0
	mov      x7, #0
	mov      x8, #0
	mov      x9, #0
	mov      x10, #0
	mov      x11, #0
	mov      x12, #0
	mov      x13, #0
	mov      x14, #0
	mov      x15, #0
	mov      x16, #0
	mov      x17, #0
	mov      x18, #0
	mov      x19, #0
	mov      x20, #0
	mov      x21, #0
	mov      x22, #0
	mov      x23, #0
	mov      x24, #0
	mov      x25, #0
	mov      x26, #0
	mov      x27, #0
	mov      x28, #0
	mov      x29, #0

	/* Enable Trapping of SIMD/FPU register for standalone BSP */
	mov      x0, #0
	orr      x0, x0, #(0x1 << 10)
	msr      CPTR_EL3, x0
	isb

	/*Set vector table base address*/
	ldr	x1, =_vector_table
	msr	VBAR_EL3,x1
	/*
	 * Clear FPUStatus variable to make sure that it contains current
	 * status of FPU i.e. disabled. In case of a warm restart execution
	 * when bss sections are not cleared, it may contain previously updated
	 * value which does not hold true now.
	 */
	ldr x0,=FPUStatus
	str xzr, [x0]
	/* Configure SCR_EL3 */
	mov      w1, #0              	//; Initial value of register is unknown
	orr      w1, w1, #(1 << 11)  	//; Set ST bit (Secure EL1 can access CNTPS_TVAL_EL1, CNTPS_CTL_EL1 & CNTPS_CVAL_EL1)
	orr      w1, w1, #(1 << 10)  	//; Set RW bit (EL1 is AArch64, as this is the Secure world)
	orr      w1, w1, #(1 << 3)   	//; Set EA bit (SError routed to EL3)
	orr      w1, w1, #(1 << 2)   	//; Set FIQ bit (FIQs routed to EL3)
	orr      w1, w1, #(1 << 1)   	//; Set IRQ bit (IRQs routed to EL3)
	msr      SCR_EL3, x1

    /* DEBUG OF ID_AA64MMFR0 */
    /*
    str x0, [sp, #-16]!
    ldr x0, =ID_AA64MMFR0_EL1_msg
    bl  uart_str
    mrs x0, ID_AA64MMFR0_EL1
    bl  uart_hex
    ldr x0, [sp], #16
    bl  uart_nl
    */

    mov x1,#0
    mrs x1,MDCR_EL3
    orr x1, x1, #(1 << 17)      // Allow event counting in secure state EL3
    msr MDCR_EL3,x1

    /* Configure CPUACTLR_EL1 */
    mrs x0, S3_1_c15_c2_0
    orr x0, x0, #0x20000000        // Forces Advanced SIMD and Floating-point clock enable active
    msr S3_1_c15_c2_0, x0

	bl 	 _startup		//jump to start

    ldr lr, [sp], #16
    ret

ID_AA64MMFR0_EL1_msg:
    .ascii "ID_AA64MMFR0_EL1: \0"

