#define ASSEMBLY

#include "gic.h"
#include "asm.h"

#undefine DEBUG

.section ".text"
.globl wait
.globl get_cur_sp
.globl get_base_sp
.globl get_stack_size
.globl proc_hang
.globl proc_hang_debug
.globl init_core
.globl configure_mmu
.globl irq_handler
.globl sync_handler

wait:
    sub x0, x0, #1
    cbnz x0, wait
    ret

get_cur_sp:
    mov x0, sp
    ret

get_base_sp:
    ldr x0, =__stack_start
    ret

get_stack_size:
    bl get_cur_sp
    mov x1, x0
    bl get_base_sp
    sub x0, x1, x0
    ret

proc_hang:
    //msr daifclr, #2
    /* wait for event */
    wfe
    bl dispatch_routine

    b proc_hang

init_core:
    mrs x0, MPIDR_EL1
    and x0, x0, 0x3
    
    cmp x0, 0x1
    bne init_core_3

    /* Assign the stack to first core */
    ldr x0, =__stack2_end
    mov sp, x0

init_core_3:
    mrs x0, MPIDR_EL1
    and x0, x0, 0x3
    cmp x0, 0x2
    bne init_core_4

    /* Assign the 3nd stack */
    ldr x0, =__stack3_end
    mov sp, x0
    
init_core_4:
    mrs x0, MPIDR_EL1
    and x0, x0, 0x3
    cmp x0, 0x3
    bne init_core_all

    /* Assign the 3nd stack */
    ldr x0, =__stack4_end
    mov sp, x0

init_core_all:
    msr daifset, #2  // Disable IRQs

    /* Activate GICC interface */
    /*
    ldr x1, =(GIC_BASE + GICC_BASE + GICC_CTRL_OFF)
    mov w0, 0x1
    str w0, [x1]
    */

    /* Set lowest GICC Interface priority */
    /*
    ldr x1, =(GIC_BASE + GICC_BASE + GICC_PMR_OFF)
    mov w0, 0xFF
    str w0, [x1]
    */

    b proc_hang

configure_mmu:
    str lr, [sp, #-16]!

    mrs x1, MPIDR_EL1 
    and x1, x1, 0x3
    ldr x0, =table_addr
    ldr x0, [x0, x1, lsl #3]
    cbz x0, skip_mmu            // If the address is null don't set the MMU

    msr TTBR0_EL3, x0
    dsb sy
    isb

    /* Configure MMU */
	ldr x1, =0x000000BB0400FF44
	msr MAIR_EL3, x1

    mrs x0, TCR_EL3
    bic x0, x0, #(1 << 20)
    bic x0, x0, #(7 << 16)
    orr x0, x0, #(4 << 16)      // 44 bits -> 16TB Physical Address Size
    bic x0, x0, #(3 << 14)      // 4KB Granularity
    orr x0, x0, #(3 << 12)      // Inner Shareable
    bic x0, x0, #(3 << 10)      
    orr x0, x0, #(1 << 10)      // Outer Normal Mem, Write-Back Read & Write Allocate Cacheable
    bic x0, x0, #(3 << 8)      
    orr x0, x0, #(1 << 8)       // Inner Normal Mem, Write-Back Read & Write Allocate Cacheable
    bic x0, x0, #63      
    ldr x1, =16
    orr x0, x0, x1             // T0SZ = 16
    msr TCR_EL3, x0
	isb

	/* Configure SCTLR_EL3 */
	mov x0, #0              //Most of the SCTLR_EL3 bits are unknown at reset
	orr x0, x0, #(1 << 0)	//Enable MMU
	msr SCTLR_EL3, x0
	dsb sy
	isb

    //bl  clean_invalidate_dcache
    //bl  clean_invalidate_icache

    tlbi alle3
    dsb sy
    isb

skip_mmu:
    ldr lr, [sp], #16
    ret

irq_handler:
    msr daifset, #2

    /* Read the interrupt ID */
    ldr x1, =(GIC_BASE + GICC_BASE + GICC_IAR_OFF)
    ldr w0, [x1]
    str w0, [sp, #-16]!
    
    and w0, w0, #0x3FF
    cbnz    w0, irq_exception

    ldr w0, [sp], #16
    /* Mark interrupt as handled */
    ldr x1, =(GIC_BASE + GICC_BASE + GICC_EOIR_OFF)
    str w0, [x1]

    /* Resetting core */
    b   _start

irq_exception:
    str w0, [sp, #-16]!
    ldr x0, =IRQ_OTHER
    bl uart_str
    ldr w0, [sp], #16
    bl uart_int
    str w0, [sp, #-16]!
    bl uart_nl

    ldr x0, =irq_handler2
    ldr x0, [x0]
    cbz x0, irq_skip
    blr x0

irq_skip:
    ldr w0, [sp], #16
    /* Mark interrupt as handled */
    ldr x1, =(GIC_BASE + GICC_BASE + GICC_EOIR_OFF)
    str w0, [x1]

    eret


sync_handler:
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
    str x30, [sp, #-16]!

    ldr x0, =ERR
    bl  uart_str
    mrs x0, MPIDR_EL1
    and x0, x0, #0x3
    bl  uart_int
    bl  uart_nl
    ldr x0, =ESR_MSG
    bl  uart_str
    mrs x0, ESR_EL3
    bl  uart_hex
    bl  uart_nl

    ldr x0, =ELR_MSG
    bl  uart_str
    mrs x0, ELR_EL3
    bl  uart_hex
    bl  uart_nl

    ldr x0, =MSG_X30
    bl  uart_str
    ldr x30, [sp], #16
    mov x0, x30
    bl uart_hex
    bl uart_nl

    ldr x0, =MSG_X29
    bl  uart_str
    ldp x28, x29, [sp], #16
    mov x0, x29
    str x28, [sp, #-16]
    bl uart_hex
    bl uart_nl
    ldr x0, =MSG_X28
    bl  uart_str
    ldr x28, [sp], #16
    mov x0, x28
    bl  uart_hex
    bl  uart_nl

    ldr x0, =MSG_X27
    bl  uart_str
    ldp x26, x27, [sp], #16
    mov x0, x27
    str x26, [sp, #-16]
    bl uart_hex
    bl uart_nl
    ldr x0, =MSG_X26
    bl  uart_str
    ldr x26, [sp], #16
    mov x0, x26
    bl  uart_hex
    bl  uart_nl

    ldr x0, =MSG_X25
    bl  uart_str
    ldp x24, x25, [sp], #16
    mov x0, x25
    str x24, [sp, #-16]
    bl uart_hex
    bl uart_nl
    ldr x0, =MSG_X24
    bl  uart_str
    ldr x24, [sp], #16
    mov x0, x24
    bl  uart_hex
    bl  uart_nl

    ldr x0, =MSG_X23
    bl  uart_str
    ldp x22, x23, [sp], #16
    mov x0, x23
    str x22, [sp, #-16]
    bl uart_hex
    bl uart_nl
    ldr x0, =MSG_X22
    bl  uart_str
    ldr x22, [sp], #16
    mov x0, x22
    bl  uart_hex
    bl  uart_nl
    
    ldr x0, =MSG_X21
    bl  uart_str
    ldp x20, x21, [sp], #16
    mov x0, x21
    str x20, [sp, #-16]
    bl uart_hex
    bl uart_nl
    ldr x0, =MSG_X20
    bl  uart_str
    ldr x20, [sp], #16
    mov x0, x20
    bl  uart_hex
    bl  uart_nl

    ldr x0, =MSG_X19
    bl  uart_str
    ldp x18, x19, [sp], #16
    mov x0, x19
    str x18, [sp, #-16]
    bl uart_hex
    bl uart_nl
    ldr x0, =MSG_X18
    bl  uart_str
    ldr x18, [sp], #16
    mov x0, x18
    bl  uart_hex
    bl  uart_nl

    ldr x0, =MSG_X17
    bl  uart_str
    ldp x16, x17, [sp], #16
    mov x0, x17
    str x16, [sp, #-16]
    bl uart_hex
    bl uart_nl
    ldr x0, =MSG_X16
    bl  uart_str
    ldr x16, [sp], #16
    mov x0, x16
    bl  uart_hex
    bl  uart_nl

    ldr x0, =MSG_X15
    bl  uart_str
    ldp x14, x15, [sp], #16
    mov x0, x15
    str x14, [sp, #-16]
    bl uart_hex
    bl uart_nl
    ldr x0, =MSG_X14
    bl  uart_str
    ldr x14, [sp], #16
    mov x0, x14
    bl  uart_hex
    bl  uart_nl

    ldr x0, =MSG_X13
    bl  uart_str
    ldp x12, x13, [sp], #16
    mov x0, x13
    str x12, [sp, #-16]
    bl uart_hex
    bl uart_nl
    ldr x0, =MSG_X12
    bl  uart_str
    ldr x12, [sp], #16
    mov x0, x12
    bl  uart_hex
    bl  uart_nl

    ldr x0, =MSG_X11
    bl  uart_str
    ldp x10, x11, [sp], #16
    mov x0, x11
    str x10, [sp, #-16]
    bl uart_hex
    bl uart_nl
    ldr x0, =MSG_X10
    bl  uart_str
    ldr x10, [sp], #16
    mov x0, x10
    bl  uart_hex
    bl  uart_nl

    ldr x0, =MSG_X9
    bl  uart_str
    ldp x8, x9, [sp], #16
    mov x0, x9
    str x8, [sp, #-16]
    bl uart_hex
    bl uart_nl
    ldr x0, =MSG_X8
    bl  uart_str
    ldr x8, [sp], #16
    mov x0, x8
    bl  uart_hex
    bl  uart_nl

    ldr x0, =MSG_X7
    bl  uart_str
    ldp x6, x7, [sp], #16
    mov x0, x7
    str x6, [sp, #-16]
    bl uart_hex
    bl uart_nl
    ldr x0, =MSG_X6
    bl  uart_str
    ldr x6, [sp], #16
    mov x0, x6
    bl  uart_hex
    bl  uart_nl

    ldr x0, =MSG_X5
    bl  uart_str
    ldp x4, x5, [sp], #16
    mov x0, x5
    str x4, [sp, #-16]
    bl uart_hex
    bl uart_nl
    ldr x0, =MSG_X4
    bl  uart_str
    ldr x4, [sp], #16
    mov x0, x4
    bl  uart_hex
    bl  uart_nl

    ldr x0, =MSG_X3
    bl  uart_str
    ldp x2, x3, [sp], #16
    mov x0, x3
    str x2, [sp, #-16]
    bl uart_hex
    bl uart_nl
    ldr x0, =MSG_X2
    bl  uart_str
    ldr x2, [sp], #16
    mov x0, x2
    bl  uart_hex
    bl  uart_nl

    ldr x0, =MSG_X1
    bl  uart_str
    ldp x0, x1, [sp], #16
    str x0, [sp, #-16]
    mov x0, x1
    bl uart_hex
    bl uart_nl
    ldr x0, =MSG_X0
    bl  uart_str
    ldr x0, [sp], #16
    bl  uart_hex
    bl  uart_nl

    mov x0, xzr
    bl outbyte

sync_loop:
    b sync_loop

ERR:
    .string "Error detected on core "
MSG:
    .string "Value of IRQ_STATUS2: "
MSG10:
    .string "Value of GICD_ISPENDR0: "
MSG11:
    .string "Value of GICD_ISPENDR1: "
MSG12:
    .string "Value of GICD_ISPENDR2: "
MSG3:
    .string "Value of GICC_HPPIR: "
MSG4:
    .string "Value of GICC_RPR: "
RESETTING_MSG:
    .string "Resetting Core "
IRQ_MSG:
    .string "IRQ DETECTED "
IRQ_CORE_ID:
    .string "IRQ DETECTED ON CORE "
IRQ_ID:
    .string "Value of GICC_IAR: "
IRQ_OTHER:
    .string "IRQ ID: "
ESR_MSG:
    .string "ESR_EL3: "
ELR_MSG:
    .string "ELR_EL3: "

MSG_X30:
    .string "X30: "
MSG_X29:
    .string "X29: "
MSG_X28:
    .string "X28: "
MSG_X27:
    .string "X27: "
MSG_X26:
    .string "X26: "
MSG_X25:
    .string "X25: "
MSG_X24:
    .string "X24: "
MSG_X23:
    .string "X23: "
MSG_X22:
    .string "X22: "
MSG_X21:
    .string "X21: "
MSG_X20:
    .string "X20: "
MSG_X19:
    .string "X19: "
MSG_X18:
    .string "X18: "
MSG_X17:
    .string "X17: "
MSG_X16:
    .string "X16: "
MSG_X15:
    .string "X15: "
MSG_X14:
    .string "X14: "
MSG_X13:
    .string "X13: "
MSG_X12:
    .string "X12: "
MSG_X11:
    .string "X11: "
MSG_X10:
    .string "X10: "
MSG_X9:
    .string "X9: "
MSG_X8:
    .string "X8: "
MSG_X7:
    .string "X7: "
MSG_X6:
    .string "X6: "
MSG_X5:
    .string "X5: "
MSG_X4:
    .string "X4: "
MSG_X3:
    .string "X3: "
MSG_X2:
    .string "X2: "
MSG_X1:
    .string "X1: "
MSG_X0:
    .string "X0: "
