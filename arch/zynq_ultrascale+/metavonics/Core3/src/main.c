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
#if !defined(HEADER)
#define HEADER ; ; 
#endif
#if !defined(TARGET_SIZE)
#define TARGET_SIZE 4*128*64
#endif
#if !defined(STRIDE)
#define STRIDE 64
#endif

#define ENEMY

#include "definitions.h"

extern u8 __buf_start_3;

int main(int argc, char *argv[]) {
    register volatile u8 *target = &__buf_start_3;
    for (HEADER) {
         for (register int i = 0 ; i < TARGET_SIZE ; i += STRIDE) {
            ACCESS_METHOD(target+i);
        }
    }
}

