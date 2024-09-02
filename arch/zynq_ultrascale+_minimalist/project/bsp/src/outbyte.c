#include <stdint.h>

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

    //for(int i = 0; i<100000; i++) ;
	 Mem_Out32(((STDOUT_BASEADDRESS) + ((uint32_t)UARTPS_FIFO_OFFSET)), (uint32_t) c);
}
