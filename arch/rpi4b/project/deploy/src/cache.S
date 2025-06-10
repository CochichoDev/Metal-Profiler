#define ASSEMBLY

.globl clean_invalidate_dcache
.globl clean_invalidate_icache

dcache_level:
	lsl	x12, x0, #1
	msr	csselr_el1, x12		/* select cache level */
	isb				/* sync change of cssidr_el1 */
	mrs	x6, ccsidr_el1		/* read the new cssidr_el1 */
	and	x2, x6, #7		/* x2 <- log2(cache line size)-4 */
	add	x2, x2, #4		/* x2 <- log2(cache line size) */
	mov	x3, #0x3ff
	and	x3, x3, x6, lsr #3	/* x3 <- max number of #ways */
	clz	w5, w3			/* bit position of #ways */
	mov	x4, #0x7fff
	and	x4, x4, x6, lsr #13	/* x4 <- max number of #sets */
	/* x12 <- cache level << 1 */
	/* x2 <- line length offset */
	/* x3 <- number of cache ways - 1 */
	/* x4 <- number of cache sets - 1 */
	/* x5 <- bit position of #ways */

loop_set:
	mov	x6, x3			/* x6 <- working copy of #ways */
loop_way:
	lsl	x7, x6, x5
	orr	x9, x12, x7		/* map way and level to cisw value */
	lsl	x7, x4, x2
	orr	x9, x9, x7		/* map set number to cisw value */
1:	dc	isw, x9		/* invalidate by set/way */
2:	subs	x6, x6, #1		/* decrement the way */
	b.ge	loop_way
	subs	x4, x4, #1		/* decrement the set */
	b.ge	loop_set

	ret

clean_invalidate_dcache:
	dsb	sy
	mrs	x10, clidr_el1		/* read clidr_el1 */
	lsr	x11, x10, #24
	and	x11, x11, #0x7		/* x11 <- loc */
	cbz	x11, finished		/* if loc is 0, exit */
	mov	x15, lr
	mov	x0, #0			/* start flush at cache level 0 */
	/* x0  <- cache level */
	/* x10 <- clidr_el1 */
	/* x11 <- loc */
	/* x15 <- return address */

loop_level:
	lsl	x12, x0, #1
	add	x12, x12, x0		/* x0 <- tripled cache level */
	lsr	x12, x10, x12
	and	x12, x12, #7		/* x12 <- cache type */
	cmp	x12, #2
	b.lt	skip			/* skip if no cache or icache */
	bl	dcache_level	/* x1 = 0 flush, 1 invalidate */
skip:
	add	x0, x0, #1		/* increment cache level */
	cmp	x11, x0
	b.gt	loop_level

	mov	x0, #0
	msr	csselr_el1, x0		/* restore csselr_el1 */
	dsb	sy
	isb
	mov	lr, x15

finished:
	ret

clean_invalidate_icache:
    ic	ialluis
	isb	sy
	ret

//clean_invalidate_cache:
//    str     lr, [sp, #-16]!
//    mrs     x0, CLIDR_EL1
//    lsr     x1, x0, #24              // Move bits [26:24] to [2:0]
//    and     x1, x1, #0x7             // Mask to get LoC (3 bits)
//    cbz     x1, dcache_done
//
//    mov     x2, #0                    // x2 = level counter
//
//check_next_level:
//    // Extract 3 bits describing cache type for this level
//    mov     x4, #3
//    mul     x3, x2, x4
//    lsr     x3, x0, x3
//    and     x3, x3, #0x7
//
//    // If type == 2 (data cache) or 4 (unified), clean it
//    cmp     x3, #2
//    b.lt    skip_level
//
//do_dcache:
//    #ifdef DEBUG
//    // Save x0 to the stack before calling uart_str
//    stp     x0, x1, [sp, #-16]!        // Save x0 and x1 to stack
//    stp     x2, x3, [sp, #-16]!
//
//    // Debug: "Cleaning level: x"
//    ldr     x0, =str_level
//    bl      uart_str
//    mov     x0, x2
//    bl      uart_hex
//    bl      uart_nl
//
//    // Restore x0 from the stack
//    ldp     x2, x3, [sp], #16         // Restore x2 and x3 from stack
//    ldp     x0, x1, [sp], #16         // Restore x0 and x1 from stack
//    #endif
//
//    // Select level in CSSELR_EL1
//    mov     x4, x2
//    lsl     x4, x4, #1                // Level << 1 (Select data/unified)
//    msr     CSSELR_EL1, x4
//    isb
//
//    mrs     x5, CCSIDR_EL1              // x5 = CCSIDR_EL1
//
//    // LineSize = [2:0], add 4 gives log2(line size in bytes)
//    and     x6, x5, #0x7
//    add     x6, x6, #4                  // x6 = log2(line size)
//    mov     x7, #1
//    lsl     x7, x7, x6                  // x7 = line size in bytes
//
//    #ifdef DEBUG
//    // Save x0 to the stack before calling uart_str for line size
//    stp     x0, x1, [sp, #-16]!        // Save x0 and x1 to stack
//    stp     x2, x3, [sp, #-16]!
//    stp     x4, x5, [sp, #-16]!
//    stp     x6, x7, [sp, #-16]!
//
//    // Debug: line size
//    ldr     x0, =str_linesz
//    bl      uart_str
//    mov     x0, x7
//    bl      uart_hex
//    bl      uart_nl
//
//    // Restore x0 from the stack
//    ldp     x6, x7, [sp], #16
//    ldp     x4, x5, [sp], #16
//    ldp     x2, x3, [sp], #16
//    ldp     x0, x1, [sp], #16         // Restore x0 and x1 from stack
//    #endif
//
//    // Sets = [27:13]
//    ubfx    x8, x5, #13, #15
//    add     x8, x8, #1                  // x8 = number of sets
//
//    // Ways = [12:3]
//    ubfx    x9, x5, #3, #10
//    add     x9, x9, #1                  // x9 = number of ways
//
//    #ifdef DEBUG
//    // Save x0 to the stack before calling uart_str for sets and ways
//    stp     x0, x1, [sp, #-16]!        // Save x0 and x1 to stack
//    stp     x2, x3, [sp, #-16]!        // Save x0 and x1 to stack
//    stp     x4, x5, [sp, #-16]!
//    stp     x6, x7, [sp, #-16]!
//    stp     x8, x9, [sp, #-16]!
//
//    // Debug: sets and ways
//    ldr     x0, =str_sets
//    bl      uart_str
//    mov     x0, x8
//    bl      uart_hex
//    bl      uart_nl
//
//    ldr     x0, =str_ways
//    bl      uart_str
//    mov     x0, x9
//    bl      uart_hex
//    bl      uart_nl
//
//    // Restore x0 from the stack
//    ldp     x8, x9, [sp], #16
//    ldp     x6, x7, [sp], #16
//    ldp     x4, x5, [sp], #16
//    ldp     x2, x3, [sp], #16
//    ldp     x0, x1, [sp], #16         // Restore x0 and x1 from stack
//    #endif
//
//    clz     x10, x9                     // x10 = way_shift
//    sub     x10, x10, #32
//    //      0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0100
//
//    mov     x11, #0                   // x11 = way index
//
//way_loop:
//    mov     x12, #0                   // x12 = set index
//
//set_loop:
//    lsl     x13, x2, #1               // level << 1
//    lsl     x14, x11, x10             // way << way_shift
//    orr     x13, x13, x14
//    lsl     x14, x12, x6              // set << log2(line size)
//    orr     x13, x13, x14             // x13 = final virtual address
//
//    #ifdef DEBUG
//    // Save x0 to the stack before calling uart_str for operand
//    stp     x0, x1, [sp, #-16]!        // Save x0 and x1 to stack
//    stp     x2, x3, [sp, #-16]!
//    stp     x4, x5, [sp, #-16]!
//    stp     x6, x7, [sp, #-16]!
//    stp     x8, x9, [sp, #-16]!
//    stp     x10, x11, [sp, #-16]!
//    stp     x12, x13, [sp, #-16]!
//
//    // Debug: print operand
//    mov     x0, x13
//    bl      uart_hex
//    bl      uart_nl
//
//    // Restore x0 from the stack
//    ldp     x12, x13, [sp], #16
//    ldp     x10, x11, [sp], #16
//    ldp     x8, x9, [sp], #16
//    ldp     x6, x7, [sp], #16
//    ldp     x4, x5, [sp], #16
//    ldp     x2, x3, [sp], #16
//    ldp     x0, x1, [sp], #16         // Restore x0 and x1 from stack
//    #endif
//
//    dc      cisw, x13
//
//    add     x12, x12, #1
//    cmp     x12, x8
//    blt     set_loop
//
//    add     x11, x11, #1
//    cmp     x11, x9
//    blt     way_loop
//
//skip_level:
//    add     x2, x2, #1
//    cmp     x2, x1
//    blt     check_next_level
//
//dcache_done:
//    dsb     sy
//
//    #ifdef DEBUG
//    // Invalidate instruction cache
//    ldr     x0, =str_icache
//    bl      uart_str
//    bl      uart_nl
//    #endif
//    ic      iallu
//    dsb     sy
//    isb
//
//    ldr     lr, [sp], #16
//    ret

// Strings in read-only section
.section .rodata
str_level:   .asciz "Cleaning level: "
str_linesz:  .asciz "Line size: "
str_sets:    .asciz "Number of sets: "
str_ways:    .asciz "Number of ways: "
str_dcval:   .asciz "dc cisw operand: "
str_icache:  .asciz "Invalidating instruction cache"

