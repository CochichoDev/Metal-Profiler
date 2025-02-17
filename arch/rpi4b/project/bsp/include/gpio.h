#pragma once

#include "common.h"

#define GPIO_OFF        (0x2200000U)

#define INPUT_FUNC      (0x0U)
#define OUTPUT_FUNC     (0x1U)
#define ALT0_FUNC       (0x4U)
#define ALT1_FUNC       (0x5U)
#define ALT2_FUNC       (0x6U)
#define ALT3_FUNC       (0x7U)
#define ALT4_FUNC       (0x3U)
#define ALT5_FUNC       (0x2U)

#define NO_RES          (0x0U)
#define PU_RES          (0x1U)
#define PD_RES          (0x2U)

#define GPIO_UART_TX0   14
#define GPIO_UART_RX0   15
#define GPIO_UART_TX4   8
#define GPIO_UART_RX4   9

#define GPIO_GPFSEL_OFF (0x0U)
#define GPIO_GPSET_OFF  (0x1CU)
#define GPIO_GPCLR_OFF  (0x28U)
#define GPIO_PU_PD_OFF  (0xE4U)

#define REG_GPIO_GPFSEL(IDX)    ( (volatile u32 *) (PBASE + GPIO_OFF + GPIO_GPFSEL_OFF + ((uintptr_t)(IDX) / 10) * 0x4U) )
#define REG_GPIO_GPSET(IDX)     ( (volatile u32 *) (PBASE + GPIO_OFF + GPIO_GPSET_OFF + ((uintptr_t)(IDX) / 32) * 0x4U) )
#define REG_GPIO_GPCLR(IDX)     ( (volatile u32 *) (PBASE + GPIO_OFF + GPIO_GPCLR_OFF + ((uintptr_t)(IDX) / 32) * 0x4U) )
#define REG_GPIO_PU_PD(IDX)     ( (volatile u32 *) (PBASE + GPIO_OFF + GPIO_GPCLR_OFF + ((uintptr_t)(IDX) / 16) * 0x4U) )

#define SHIFT_GPIO_GPFSEL(IDX)  ( (IDX % 10) * 0x3U )
#define SHIFT_GPIO_GPSET(IDX)   ( (IDX % 32) )
#define SHIFT_GPIO_GPCLR(IDX)   ( (IDX % 32) )
#define SHIFT_GPIO_PU_PD(IDX)   ( (IDX % 16) * 0x2U )

void sel_gpio_func(u8 gpio_num, u8 gpio_func);
void set_gpio_output(u8 gpio_num);
void clr_gpio_output(u8 gpio_num);
void conf_gpio_pupd(u8 gpio_num, u8 pupd);
