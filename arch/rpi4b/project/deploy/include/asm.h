#pragma once

#include "common.h"

#ifndef ASSEMBLY
void wait(u64 cycles);
u64 get_cur_sp();
u64 get_base_sp();
u64 get_stack_size();
void call_dyn_func();

#define save_reg(SP_SAVE)    \
    __asm__("  \
            stp x0, x1, [sp, #-16]!\n  \
            stp x2, x3, [sp, #-16]!\n  \
            stp x4, x5, [sp, #-16]!\n  \
            stp x6, x7, [sp, #-16]!\n  \
            stp x8, x9, [sp, #-16]!\n  \
            stp x10, x11, [sp, #-16]!\n    \
            stp x12, x13, [sp, #-16]!\n    \
            stp x14, x15, [sp, #-16]!\n    \
            stp x16, x17, [sp, #-16]!\n    \
            stp x18, x19, [sp, #-16]!\n    \
            stp x20, x21, [sp, #-16]!\n    \
            stp x22, x23, [sp, #-16]!\n    \
            stp x24, x25, [sp, #-16]!\n    \
            stp x26, x27, [sp, #-16]!\n    \
            stp x28, x29, [sp, #-16]!\n    \
            str x30, [sp, #-16]!\n         \
            mov x0, sp\n                   \
            str x0, [%0]                  \
            " : : "r" (&SP_SAVE) : "memory" )

#define load_reg(SP_SAVE)    \
    __asm__("  \
            ldr x0, [%0]\n            \
            mov sp, x0\n                \
            ldr x30, [sp], #16\n      \
            ldp x28, x29, [sp], #16\n   \
            ldp x26, x27, [sp], #16\n   \
            ldp x24, x25, [sp], #16\n   \
            ldp x22, x23, [sp], #16\n   \
            ldp x20, x21, [sp], #16\n   \
            ldp x18, x19, [sp], #16\n   \
            ldp x16, x17, [sp], #16\n   \
            ldp x14, x15, [sp], #16\n   \
            ldp x12, x13, [sp], #16\n   \
            ldp x10, x11, [sp], #16\n   \
            ldp x8, x9, [sp], #16\n  \
            ldp x6, x7, [sp], #16\n  \
            ldp x4, x5, [sp], #16\n  \
            ldp x2, x3, [sp], #16\n  \
            ldp x0, x1, [sp], #16\n  \
            " : : "r" (&SP_SAVE) : "memory" )

#define lock_mutex(LOCK)                          \
    __asm__ volatile (                            \
        "1: ldxr    w0, [%0]    \n"               /* Load the lock value into w0 */ \
        "   cbnz    w0, 1b      \n"               /* If w0 is non-zero (locked), retry */ \
        "   mov     w0, #1      \n"               /* Set w0 to 1 (acquire the lock) */ \
        "   stxr    w1, w0, [%0] \n"              /* Attempt to store 1 in LOCK (w1 = status) */ \
        "   cbnz    w1, 1b      \n"               /* If store failed, retry */ \
        "   dmb     ish          \n"              /* Ensure memory ordering */ \
        : : "r" (&LOCK)                          /* Input: address of the lock */ \
        : "w0", "w1", "memory"                   /* Clobbered registers: w0, w1, memory */ \
    );

#define unlock_mutex(LOCK)                        \
    __asm__ volatile (                            \
        "dmb     ish       \n"                    /* Ensure all memory operations are complete */ \
        "str     wzr, [%0] \n"                    /* Store 0 in the lock (unlock) */ \
        : : "r" (&LOCK)                          /* Input: address of the lock */ \
        : "memory"                                /* Clobber memory, no need for register clobber */ \
    );
            
#else
.macro save_reg reg
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
    mov x9, sp
    str x9, [\reg]
.endm
.macro load_reg reg
    ldr x9, [\reg]
    mov sp, x9
    ldr x30, [sp], #16
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
.macro disable_mmu
    mov x0, #0
    msr SCTLR_EL3, x0
    dsb sy
    isb

    msr TTBR0_EL3, x0
    dsb sy
    isb
.endm
#endif
