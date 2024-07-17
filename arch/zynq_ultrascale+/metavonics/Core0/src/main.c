#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_cache.h"
#include "PMU.h"
#include "xtime_l.h"
#include "xil_mmu.h"
#include <time.h>
#include <assert.h>
#include <math.h>

#include <unistd.h>

/*
 *  DUMMY DEFINITIONS FOR WARNING AVOIDANCE
 */
#if !defined(ACCESS_METHOD)
#define ACCESS_METHOD(TARGET) 
#endif
#if !defined(TARGET_SIZE)
#define TARGET_SIZE 4*128*64
#endif
#if !defined(STRIDE)
#define STRIDE 64
#endif
#if !defined(LIMIT)
#define LIMIT 50
#endif

#define VICTIM

#include "definitions.h"

extern char __text_start;
extern char __text_end;
extern u8 __buf_start_0;


__attribute__((section(".bench"))) int main(int argc, char *argv[]) {
    register volatile u8 *target = &__buf_start_0;
    Xil_SetTlbAttributesRange(&__text_start, &__text_end, NORM_NONCACHE);
    INIT();
    for (HEADER) {
        PREP();
         for (register int i = 0 ; i < TARGET_SIZE ; i += STRIDE) {
            ACCESS_METHOD(target+i);
        }
        RETRIEVE();
    }
    END();
}

