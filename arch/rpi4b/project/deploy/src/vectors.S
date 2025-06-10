#define ASSEMBLY

#include "asm.h"

.section .vectors, "ax"

.globl _vtable
.globl dispatch_routine
.globl rst_addr
.globl irq_handler2
.globl table_addr

_vtable:
    .set VBAR, _vtable
    .org VBAR
    b _sync_exception

    .org (VBAR+0x80)
    b _irq_exception

    .org (VBAR+0x100)
    b _fiq_exception

    .org (VBAR+0x180)
    b _serror_exception

    .org (VBAR+0x200)
    b _sync_exception

    .org (VBAR+0x280)
    b _irq_exception

    .org (VBAR+0x300)
    b _fiq_exception

    .org (VBAR+0x380)
    b _serror_exception

    .org (VBAR+0x400)
    b _sync_exception

    .org (VBAR+0x480)
    b _irq_exception

    .org (VBAR+0x500)
    b _fiq_exception

    .org (VBAR+0x580)
    b _serror_exception

    .org (VBAR+0x600)
    b _sync_exception

    .org (VBAR+0x680)
    b _irq_exception

    .org (VBAR+0x700)
    b _fiq_exception

    .org (VBAR+0x780)
    b _serror_exception

.align 0x8
dispatch_routine:
    str lr, [sp, #-16]!

	mrs x1, MPIDR_EL1
	and x1, x1, #0x3
    ldr x0, =sp_save
    add x0, x0, x1, lsl #3
    save_reg x0

	mrs x1, MPIDR_EL1
	and x1, x1, #0x3
	ldr x0, =rst_addr
    ldr x0, [x0, x1, lsl #3]

    str x0, [sp, #-16]!

    ldr x1, =proc_hang
    cmp x1, x0
    beq skip_dispatch

	mrs x1, MPIDR_EL1
	and x1, x1, #0x3
    cbz x1, dispatch     // Skip if it's Core #0 (Already activated)
    bl  configure_mmu
dispatch:
    /* Activate caches */
    mrs x0, SCTLR_EL3
	orr x0, x0, #(1 << 12)	//Enable I cache
	orr x0, x0, #(1 << 2)	//Enable caches
    msr SCTLR_EL3, x0
    dsb sy
    isb

    ldr x0, [sp], #16
    blr  x0
skip_dispatch:
    /* Recover previously saved registers */
	mrs x1, MPIDR_EL1
	and x1, x1, #0x3
    ldr x0, =sp_save
    add x0, x0, x1, lsl #3
    load_reg x0

    ldr lr, [sp], #16
    ret

.align 0x8
rst_addr:
    .quad   proc_hang
    .quad   proc_hang
    .quad   proc_hang
    .quad   proc_hang

.align 0x8
sp_save:
    .skip   0x8
    .skip   0x8
    .skip   0x8
    .skip   0x8

.align 0x8
irq_handler2:
    .quad 0x0

.align 0x8
table_addr:
    .quad   0x0
    .quad   0x0
    .quad   0x0
    .quad   0x0
