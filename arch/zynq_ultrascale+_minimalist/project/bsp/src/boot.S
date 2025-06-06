#include "xparameters.h"
#include "bspconfig.h"

.extern __text_start
.extern __text_end
.extern __ld_text
.extern __rodata_start
.extern __rodata_end
.extern __ld_rodata
.extern __data_start
.extern __data_end
.extern __ld_data

.extern __loader_start

.extern MMUTableL0.0

.global _prestart
.global _boot

.extern __stack_end
.global _vector_table

.set EL3_stack,		__stack_end

.set TT_S1_FAULT,	0x0
.set TT_S1_TABLE,	0x3

.set L0Table,	MMUTableL0.0

.set vector_base,	_vector_table
.set rvbar_base,	0xFD5C0040

.set counterfreq,	XPAR_CPU_CORTEXA53_0_TIMESTAMP_CLK_FREQ

.section .boot,"ax"


/* this initializes the various processor modes */
.text_start:
    .quad __text_start
.text_end:
    .quad  __text_end
.text_ld:
    .quad  __ld_text
.rodata_start:
    .quad __rodata_start
.rodata_end:
    .quad  __rodata_end
.rodata_ld:
    .quad  __ld_rodata
.data_start:
    .quad __data_start
.data_end:
    .quad  __data_end
.data_ld:
    .quad  __ld_data
.loader:
    .quad  __loader_start

_prestart:
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
#if 0 //don't put other a53 cpus in wfi
   //Which core am I
   // ----------------
	mrs      x0, MPIDR_EL1
	and      x0, x0, #0xFF                        //Mask off to leave Aff0
	cbz      x0, OKToRun                          //If core 0, run the primary init code
EndlessLoop0:
	wfi
	b        EndlessLoop0
#endif
OKToRun:

	mrs	x0, currentEL
	cmp	x0, #0xC
	beq	InitEL3

	b 	error			// go to error if current exception level is neither EL3 nor EL1
InitEL3:
.if (EL3 == 1)
	/*Set vector table base address*/
	ldr	x1, =vector_base
	msr	VBAR_EL3,x1

	/* Set reset vector address */
	/* Get the cpu ID */
	mrs  x0, MPIDR_EL1
	and  x0, x0, #0xFF
	mov  w0, w0
	ldr	 w2, =rvbar_base
	/* calculate the rvbar base address for particular CPU core */
	mov	 w3, #0x8
	mul	 w0, w0, w3
	add	 w2, w2, w0
	/* store vector base address to RVBAR */
	str  x1, [x2]

	/*Define stack pointer for current exception level*/
	ldr	 x2,=EL3_stack
	mov	 sp,x2

	/* Enable Trapping of SIMD/FPU register for standalone BSP */
	mov      x0, #0
#ifndef FREERTOS_BSP
	orr      x0, x0, #(0x1 << 10)
#endif
	msr      CPTR_EL3, x0
	isb

	/*
	 * Clear FPUStatus variable to make sure that it contains current
	 * status of FPU i.e. disabled. In case of a warm restart execution
	 * when bss sections are not cleared, it may contain previously updated
	 * value which does not hold true now.
	 */
#ifndef FREERTOS_BSP
	 ldr x0,=FPUStatus
	 str xzr, [x0]
#endif
	/* Configure SCR_EL3 */
	mov      w1, #0              	//; Initial value of register is unknown
	orr      w1, w1, #(1 << 11)  	//; Set ST bit (Secure EL1 can access CNTPS_TVAL_EL1, CNTPS_CTL_EL1 & CNTPS_CVAL_EL1)
	orr      w1, w1, #(1 << 10)  	//; Set RW bit (EL1 is AArch64, as this is the Secure world)
	orr      w1, w1, #(1 << 3)   	//; Set EA bit (SError routed to EL3)
	orr      w1, w1, #(1 << 2)   	//; Set FIQ bit (FIQs routed to EL3)
	orr      w1, w1, #(1 << 1)   	//; Set IRQ bit (IRQs routed to EL3)
	msr      SCR_EL3, x1

	/*configure cpu auxiliary control register EL1 */
	ldr	x0,=0x80CA000 		// L1 Data prefetch control - 5, Enable device split throttle, 2 independent data prefetch streams
#if CONFIG_ARM_ERRATA_855873
        /*
	 *  Set ENDCCASCI bit in CPUACTLR_EL1 register, to execute data
	 *  cache clean operations as data cache clean and invalidate
	 *
	 */
        orr     x0, x0, #(1 << 44)      //; Set ENDCCASCI bit
#endif
	msr	S3_1_C15_C2_0, x0 	//CPUACTLR_EL1

	/* program the counter frequency */
	ldr	x0,=counterfreq
	msr	CNTFRQ_EL0, x0

	/*Enable hardware coherency between cores*/
	mrs      x0, S3_1_c15_c2_1  	//Read EL1 CPU Extended Control Register
	orr      x0, x0, #(1 << 6)  	//Set the SMPEN bit
	msr      S3_1_c15_c2_1, x0  	//Write EL1 CPU Extended Control Register
	isb

	tlbi 	ALLE3
	ic      IALLU                  	//; Invalidate I cache to PoU
	bl 	invalidate_dcaches
	dsb	 sy
	isb

	ldr      x1, =L0Table 		//; Get address of level 0 for TTBR0_EL3
	msr      TTBR0_EL3, x1		//; Set TTBR0_EL3

	/**********************************************
	* Set up memory attributes
	* This equates to:
	* 0 = b01000100 = Normal, Inner/Outer Non-Cacheable
	* 1 = b11111111 = Normal, Inner/Outer WB/WA/RA
	* 2 = b00000000 = Device-nGnRnE
	* 3 = b00000100 = Device-nGnRE
	* 4 = b10111011 = Normal, Inner/Outer WT/WA/RA
	**********************************************/
	ldr      x1, =0x000000BB0400FF44
	msr      MAIR_EL3, x1

#if defined (versal)
	/**********************************************
	 * Set up TCR_EL3
	 * Physical Address Size PS =  100 -> 44bits 16 TB
	 * Granual Size TG0 = 00 -> 4KB
	 * size offset of the memory region T0SZ = 20 -> (region size 2^(64-20) = 2^44)
	 ***************************************************/
	ldr     x1,=0x80843514
#else
	/**********************************************
	 * Set up TCR_EL3
	 * Physical Address Size PS =  010 -> 40bits 1TB
	 * Granual Size TG0 = 00 -> 4KB
	 * size offset of the memory region T0SZ = 24 -> (region size 2^(64-24) = 2^40)
	 ***************************************************/
	ldr     x1,=0x80823518
#endif

#if defined (CACHECOLORING)
	ldr     x1,=0x80823518
#else
    // 2MB Page
    ldr     x1,=0x80823518
#endif

	msr     TCR_EL3, x1
	isb

	/* Enable SError Exception for asynchronous abort */
	mrs 	x1,DAIF
	bic	x1,x1,#(0x1<<8)
        msr	DAIF,x1

	/* Configure SCTLR_EL3 */
	mov      x1, #0                //Most of the SCTLR_EL3 bits are unknown at reset
	orr      x1, x1, #(1 << 12)	//Enable I cache
	orr      x1, x1, #(1 << 3)	//Enable SP alignment check
	orr      x1, x1, #(1 << 2)	//Enable caches
	orr      x1, x1, #(1 << 0)	//Enable MMU
	msr      SCTLR_EL3, x1
	dsb	 sy
	isb

    ldr     x3, .loader
    ldr     x0, .text_start
    ldr     x1, .text_ld
    ldr     x2, .text_end
    sub     x2, x2, x0
    blr     x3
    ldr     x0, .rodata_start
    ldr     x1, .rodata_ld
    ldr     x2, .rodata_end
    sub     x2, x2, x0
    blr     x3
    ldr     x0, .data_start
    ldr     x1, .data_ld
    ldr     x2, .data_end
    sub     x2, x2, x0
    blr     x3

	b 	 _startup		//jump to start
.else
	b 	error			// present exception level and selected exception level mismatch
.endif

error: 	b	error


invalidate_dcaches:

	dmb     ISH
	mrs     x0, CLIDR_EL1          //; x0 = CLIDR
	ubfx    w2, w0, #24, #3        //; w2 = CLIDR.LoC
	cmp     w2, #0                 //; LoC is 0?
	b.eq    invalidateCaches_end   //; No cleaning required and enable MMU
	mov     w1, #0                 //; w1 = level iterator

invalidateCaches_flush_level:
	add     w3, w1, w1, lsl #1     //; w3 = w1 * 3 (right-shift for cache type)
	lsr     w3, w0, w3             //; w3 = w0 >> w3
	ubfx    w3, w3, #0, #3         //; w3 = cache type of this level
	cmp     w3, #2                 //; No cache at this level?
	b.lt    invalidateCaches_next_level

	lsl     w4, w1, #1
	msr     CSSELR_EL1, x4         //; Select current cache level in CSSELR
	isb                            //; ISB required to reflect new CSIDR
	mrs     x4, CCSIDR_EL1         //; w4 = CSIDR

	ubfx    w3, w4, #0, #3
	add    	w3, w3, #2             //; w3 = log2(line size)
	ubfx    w5, w4, #13, #15
	ubfx    w4, w4, #3, #10        //; w4 = Way number
	clz     w6, w4                 //; w6 = 32 - log2(number of ways)

invalidateCaches_flush_set:
	mov     w8, w4                 //; w8 = Way number
invalidateCaches_flush_way:
	lsl     w7, w1, #1             //; Fill level field
	lsl     w9, w5, w3
	orr     w7, w7, w9             //; Fill index field
	lsl     w9, w8, w6
	orr     w7, w7, w9             //; Fill way field
	dc      CISW, x7               //; Invalidate by set/way to point of coherency
	subs    w8, w8, #1             //; Decrement way
	b.ge    invalidateCaches_flush_way
	subs    w5, w5, #1             //; Descrement set
	b.ge    invalidateCaches_flush_set

invalidateCaches_next_level:
	add     w1, w1, #1             //; Next level
	cmp     w2, w1
	b.gt    invalidateCaches_flush_level

invalidateCaches_end:
	ret

.end
