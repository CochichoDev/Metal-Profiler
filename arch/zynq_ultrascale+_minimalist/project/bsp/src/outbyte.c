#include "io.h"
#include "outbyte.h"
#include "xparameters.h"

#ifdef __cplusplus
extern "C" {
#endif
void outbyte(char c); 

#ifdef __cplusplus
}
#endif 


void outbyte(char c) {
    
    while (UART_IsTransmissionFull(STDOUT_BASEADDRESS))
        ;

	 Mem_Out8(((STDOUT_BASEADDRESS) + ((uint32_t)UARTPS_FIFO_OFFSET)), c);
}
