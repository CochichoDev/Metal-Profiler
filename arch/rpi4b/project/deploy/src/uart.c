#include "asm.h"
#include "common.h"
#include "gpio.h"
#include "uart.h"
#include "utils.h"
#include "mbox.h"


static void uart_get_divisor(u32 div[], u32 baudrate) {
    u32 uart_freq = mbox_get_uart_freq();

    uart_str("UART FREQ: "); uart_int(uart_freq); uart_nl();

    div[0] = uart_freq / (baudrate << 4);
    div[1] = (( uart_freq % (baudrate << 4) ) * 10) / (baudrate << 4);
}

void init_uart(u32 baudrate) {
#if (STDOUT==UART4)
    sel_gpio_func(GPIO_UART_RX4, ALT4_FUNC);
    conf_gpio_pupd(GPIO_UART_RX4, NO_RES);
    sel_gpio_func(GPIO_UART_TX4, ALT4_FUNC);
    conf_gpio_pupd(GPIO_UART_TX4, NO_RES);
#endif

    /* Configure line control register (8 bit Parity) */
    *REG_UART_LCRH(STDOUT) &= UART_LCRH_MASK;
    *REG_UART_LCRH(STDOUT) |= 0x70;     //0b0111 0000

    /* Configure baudrate */
    *REG_UART_IBRD(STDOUT) = 10;
    *REG_UART_FBRD(STDOUT) = 0;

    /* Mask all interrupts */
    *REG_UART_IFLS(STDOUT) = UART_IFLS_MASK;
    *REG_UART_IMSC(STDOUT) = UART_IMSC_MASK;

    /* Configure line control (ENABLE UART) */
    *REG_UART_CR(STDOUT) &= UART_CR_MASK;
    *REG_UART_CR(STDOUT) |= 0b1100001100000001;  //0b1100 0001 0000 0001
    asm volatile("dmb sy");
}

void outbyte(char c) {
    /* Wait until transmission channel is empty */
    while(*REG_UART_FR(STDOUT) & UART_FR_TXFF_BIT);
    /* Send the character */
    *REG_UART_DR(STDOUT) = (c & UART_DR_TX_MASK);
}

char inbyte() {
    /* Wait until transmission channel is empty */
    while(*REG_UART_FR(STDOUT) & UART_FR_RXFE_BIT);
    /* Send the character */
    return *REG_UART_DR(STDOUT) & UART_DR_TX_MASK;
}

void drain_uart_input() {
    volatile char tmp = 0;
    while((*REG_UART_FR(STDOUT) & UART_FR_RXFF_BIT)) {
        tmp = (volatile u8) *REG_UART_DR(STDOUT);
    }
}

void uart_str(char *str) {
    while (*str != 0) {
        outbyte(*str);
        str++;
    }
}

void uart_int(u64 num) {
    char nbuf[32];
    itos(num, nbuf);
    uart_str(nbuf);
}

void uart_hex(u64 num) {
    char nbuf[32];
    i16tos(num, nbuf);
    uart_str(nbuf);
}

void uart_nl() {
    outbyte('\n');
}

void get_uart_input(char input[], u32 max_size) {
    char *ptr = input;
    while ((ptr - input) < max_size) {
        *ptr = inbyte();
        if (*ptr == '\r' || *ptr == '\n') break;
        ++ptr;
    }
    *ptr = 0;
}
