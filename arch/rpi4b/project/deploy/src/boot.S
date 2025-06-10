.set L0Table, MMUTableL0.0

.section ".text.boot", "x"
.globl _start
_start:
	/* Set L2 read/write cache latency to 3 */
    /* L2CTLR_EL1 */
	mrs x0, S3_1_c11_c0_2
	mov x1, #0x22
	orr x0, x0, x1
	msr S3_1_c11_c0_2, x0

	/* Set up CNTFRQ_EL0 */
	ldr x0, =54000000
	msr CNTFRQ_EL0, x0

	/* Set up CNTVOFF_EL2 */
	msr CNTVOFF_EL2, xzr

	/* Enable FP/SIMD */
	/* Bit 10 (TFP) is set to 0 */
	msr CPTR_EL3, xzr

	/* Set up SCR */
	mov x0, #0x5BF
	msr SCR_EL3, x0

	/* Set up ACTLR */
	mov x0, #0x73
	msr ACTLR_EL3, x0

	/* Set SMPEN */
    /* CPUECTLR_EL1 */
	mov x0, #0x40
	msr S3_0_C15_C1_4, x0

    // Disable MMU and caches
    mov x0, #0
    msr TTBR0_EL3, x0
    msr SCTLR_EL3, x0
    dsb sy
    isb

    ldr x0, =_vtable
    msr VBAR_EL3, x0

	mrs x0, MPIDR_EL1
	and x0, x0, #0x3
	cbnz x0, init_core

	/*
	 * LOCAL_CONTROL:
	 * Bit 9 clear: Increment by 1 (vs. 2).
	 * Bit 8 clear: Timer source is 19.2MHz crystal (vs. APB).
	 */
	ldr x0, =0xff800000
	str wzr, [x0]
	/* LOCAL_PRESCALER; divide-by (0x80000000 / register_val) == 1 */
	mov w1, 0x80000000
	str w1, [x0, #(0xff800008 - 0xff800000)]

    mrs x0, CurrentEL
    and x0, x0, #0xF
    lsr x0, x0, #2
    cmp x0, #0x3
    bne proc_hang

    ldr x0, =__bss_start
    ldr x1, =__bss_end
    cmp x1, x0
    beq bss_end
bss_loop:
    str xzr, [x0], #8
    cmp x0, x1
    bne bss_loop
    
bss_end:
    ldr x0, =__stack_end
    mov sp, x0

    bl main
    b proc_hang

