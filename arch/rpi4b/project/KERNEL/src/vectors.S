.section .vectors, "ax"

.globl rst_addr

.align 0x8
rst_addr:
    .quad   proc_hang
    .quad   proc_hang
    .quad   proc_hang
    .quad   proc_hang

