#include "bspconfig.h"

.extern MMUTableL0.0

.macro save_reg
    stp x0, x1, [sp, #-16]!
    stp x2, x3, [sp, #-16]!
    stp x4, x5, [sp, #-16]!
    stp x6, x7, [sp, #-16]!
    stp x8, x9, [sp, #-16]!
    stp x10, x11, [sp, #-16]!
    stp x12, x13, [sp, #-16]!
    stp x14, x15, [sp, #-16]!
    stp x16, x17, [sp, #-16]!
    stp x18, x19, [sp, #-16]!
    stp x20, x21, [sp, #-16]!
    stp x22, x23, [sp, #-16]!
    stp x24, x25, [sp, #-16]!
    stp x26, x27, [sp, #-16]!
    stp x28, x29, [sp, #-16]!
    stp x30, x0, [sp, #-16]!
.endm


.macro recov_reg
    ldp x30, x0, [sp], #16
    ldp x28, x29, [sp], #16
    ldp x26, x27, [sp], #16
    ldp x24, x25, [sp], #16
    ldp x22, x23, [sp], #16
    ldp x20, x21, [sp], #16
    ldp x18, x19, [sp], #16
    ldp x16, x17, [sp], #16
    ldp x14, x15, [sp], #16
    ldp x12, x13, [sp], #16
    ldp x10, x11, [sp], #16
    ldp x8, x9, [sp], #16
    ldp x6, x7, [sp], #16
    ldp x4, x5, [sp], #16
    ldp x2, x3, [sp], #16
    ldp x0, x1, [sp], #16
.endm

.global _prestart
.global _boot

.extern __stack_end
.global _vector_table

.set L0Table,	MMUTableL0.0
.set L1Table,	MMUTableL1.0

.section .boot,"ax"

_boot:
    save_reg
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
    /*
	ldr	 x2, =__stack_end
	mov	 sp, x2
    */

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

    str x0, [sp, #-16]!
    ldr x0, =ID_AA64MMFR0_EL1_msg
    bl  uart_str
    mrs x0, ID_AA64MMFR0_EL1
    bl  uart_hex
    ldr x0, [sp], #16
    bl  uart_nl

    mrs     x1, TCR_EL3
    bic     x1, x1, #(1 << 20)
    bic     x1, x1, #(7 << 16)
    orr     x1, x1, #(4 << 16)      // 44 bits -> 16TB Physical Address Size
    bic     x1, x1, #(3 << 14)      // 4KB Granularity
    orr     x1, x1, #(3 << 12)      // Inner Shareable
    bic     x1, x1, #(3 << 10)      
    orr     x1, x1, #(1 << 10)      // Outer Normal Mem, Write-Back Read & Write Allocate Cacheable
    bic     x1, x1, #(3 << 8)      
    orr     x1, x1, #(1 << 8)       // Inner Normal Mem, Write-Back Read & Write Allocate Cacheable
    bic     x1, x1, #63      
    ldr     x2, =16
    orr     x1, x1, x2             // T0SZ = 16

    stp     x0, x1, [sp, #-16]!
    mov     x0, x1
    bl      uart_hex
    bl      uart_nl
    ldp     x0, x1, [sp], #16

    msr     TCR_EL3, x1
	isb

	/* Enable SError Exception for asynchronous abort */
	mrs x1,DAIF
	bic	x1,x1,#(0x1<<8)
    msr	DAIF,x1

    mov x1,#0
    mrs x1,MDCR_EL3
    orr x1, x1, #(1 << 17)      // Allow event counting in secure state EL3
    msr MDCR_EL3,x1

	/* Configure SCTLR_EL3 */
	mov x1, #0                //Most of the SCTLR_EL3 bits are unknown at reset
	orr x1, x1, #(1 << 12)	//Enable I cache
	orr x1, x1, #(1 << 3)	//Enable SP alignment check
	orr x1, x1, #(1 << 2)	//Enable caches
	orr x1, x1, #(1 << 0)	//Enable MMU
	msr SCTLR_EL3, x1
	dsb sy
	isb

    str lr, [sp, #-16]!
	bl 	 _startup		//jump to start
    ldr lr, [sp], #16
    recov_reg
    ret

ID_AA64MMFR0_EL1_msg:
    .ascii "ID_AA64MMFR0_EL1: \0"

