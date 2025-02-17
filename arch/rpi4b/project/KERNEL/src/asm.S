.section ".text"
.globl wait
.globl proc_hang

wait:
    sub x0, x0, #1
    cbnz x0, wait
    ret

proc_hang:
    /* wait for event */
    wfe

	mrs x0, MPIDR_EL1
	and x0, x0, #0x3

    ldr x1, =rst_addr
    ldr x0, [x1, x0, lsl #0x3]

    blr x0


    b proc_hang
