#pragma once

#define UART0               (0x2201000U)
#define UART2               (0x2201400U)
#define UART3               (0x2201600U)
#define UART4               (0x2201800U)
#define UART5               (0x2201A00U)

#define UART_DR_OFF         (0x0U)
#define UART_FR_OFF         (0x18U)
#define UART_IBRD_OFF       (0x24U)
#define UART_FBRD_OFF       (0x28U)
#define UART_LCRH_OFF       (0x2CU)
#define UART_CR_OFF         (0x30U)
#define UART_IFLS_OFF       (0x34U)
#define UART_IMSC_OFF       (0x38U)

#define REG_UART_DR(IDX)    ( (volatile u32 *) (PBASE + (uintptr_t)(IDX) + UART_DR_OFF) )
#define REG_UART_FR(IDX)    ( (volatile u32 *) (PBASE + (uintptr_t)(IDX) + UART_FR_OFF) )
#define REG_UART_IBRD(IDX)  ( (volatile u32 *) (PBASE + (uintptr_t)(IDX) + UART_IBRD_OFF) )
#define REG_UART_FBRD(IDX)  ( (volatile u32 *) (PBASE + (uintptr_t)(IDX) + UART_FBRD_OFF) )
#define REG_UART_LCRH(IDX)  ( (volatile u32 *) (PBASE + (uintptr_t)(IDX) + UART_LCRH_OFF) )
#define REG_UART_CR(IDX)    ( (volatile u32 *) (PBASE + (uintptr_t)(IDX) + UART_CR_OFF) )
#define REG_UART_IFLS(IDX)  ( (volatile u32 *) (PBASE + (uintptr_t)(IDX) + UART_IFLS_OFF) )
#define REG_UART_IMSC(IDX)  ( (volatile u32 *) (PBASE + (uintptr_t)(IDX) + UART_IMSC_OFF) )

#define UART_LCRH_MASK      (0xFFFFFF00U)
#define UART_CR_MASK        (0xFFFF0078U)
#define UART_DR_TX_MASK     (0xFFU)
#define UART_IFLS_MASK      (0x0U)
#define UART_IMSC_MASK      (0x7FFU)

#define UART_FR_TXFF_BIT    (0x20U)
#define UART_FR_RXFE_BIT    (0x10U)
#define UART_FR_BUSY_BIT    (0x8U)

void init_uart(u32 baudrate);
void outbyte(char c);
char inbyte();
