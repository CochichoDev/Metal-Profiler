#include "common.h"
#include "gpio.h"
#include "mbox.h"
#include "uart.h"
#include "loader.h"
#include "uart_internals.h"


int main(void) {
    mbox_set_uart_freq((BAUDRATE << 4)*10, 1);

    loader_scan_action();

    return 0;
}
