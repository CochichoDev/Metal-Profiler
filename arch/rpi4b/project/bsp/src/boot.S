#include "bspconfig.h"

.extern MMUTableL0.0

.global _prestart
.global _boot

.extern __stack_end
.global _vector_table

.set L0Table,	MMUTableL0.0

.section .boot,"ax"

_boot:
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
	mov      x30, #0
	ldr	x1, =_vector_table
	msr	VBAR_EL3, x1

	/*Define stack pointer for current exception level*/
	ldr	 x2, =__stack_end
	mov	 sp, x2

	/* Enable Trapping of SIMD/FPU register for standalone BSP */
	mov      x0, #0
	orr      x0, x0, #(0x1 << 10)
	msr      CPTR_EL3, x0
	isb

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

    /* Virtual Memory Setup */
	ldr x1, =L0Table 		//; Get address of level 0 for TTBR0_EL3
	msr TTBR0_EL3, x1		//; Set TTBR0_EL3

	ldr x1, =0x000000BB0400FF44
	msr MAIR_EL3, x1

	ldr x1,=0x80823518
	msr TCR_EL3, x1
	isb

	/* Enable SError Exception for asynchronous abort */
	mrs x1,DAIF
	bic	x1,x1,#(0x1<<8)
    msr	DAIF,x1

	/* Configure SCTLR_EL3 */
	mov x1, #0                //Most of the SCTLR_EL3 bits are unknown at reset
	orr x1, x1, #(1 << 12)	//Enable I cache
	orr x1, x1, #(1 << 3)	//Enable SP alignment check
	orr x1, x1, #(1 << 2)	//Enable caches
	orr x1, x1, #(1 << 0)	//Enable MMU
	msr SCTLR_EL3, x1
	dsb sy
	isb

	b 	 _startup		//jump to start
