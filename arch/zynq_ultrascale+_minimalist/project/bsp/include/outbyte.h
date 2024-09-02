#ifndef OUTBYTE_H
#define OUTBYTE_H

#include "io.h"
#include "xuartps_hw.h"

#define UART_IsTransmissionFull(BaseAddress) \
    ((Mem_In32((BaseAddress) + UARTPS_SR_OFFSET) & ((uint32_t)UARTPS_SR_TXFULL)) == ((uint32_t)UARTPS_SR_TXFULL))
    

#endif
