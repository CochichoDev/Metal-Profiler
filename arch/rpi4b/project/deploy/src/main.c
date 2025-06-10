#include "common.h"
#include "gic.h"
#include "gpio.h"
#include "mbox.h"
#include "uart.h"
#include "loader.h"

int main(void) {
    //mbox_set_arm_freq(2000000000, 0);
    mbox_set_uart_freq((BAUDRATE << 4)*10, 1);
    init_uart(BAUDRATE);

    /* UART Handshake */
    while (inbyte() != 0x1);
    outbyte(0x1);
    while (inbyte() != 0x0);

    /* Enable GICD */
    //*REG_GICD_CTRL = 1;

    loader_scan_action();

    return 0;
}
