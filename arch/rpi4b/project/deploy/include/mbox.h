#pragma once

#include "common.h"
#include "gpio.h"

#define MBUFFER_SIZE    (28)

#define MBOX_OFF        (0x200B880UL)

#define MBOX_RD_OFF     (0x00U)
#define MBOX_WR_OFF     (0x20U)
#define MBOX_RD_ST_OFF     (0x18U)
#define MBOX_WR_ST_OFF     (0x38U)

#define MBOX_ST_FULL_MASK   (0x80000000U)
#define MBOX_ST_EMPTY_MASK  (0x40000000U)

#define REG_MBOX_RD     ( (volatile u32 *) (PBASE + MBOX_OFF + MBOX_RD_OFF) )
#define REG_MBOX_WR     ( (volatile u32 *) (PBASE + MBOX_OFF + MBOX_WR_OFF) )
#define REG_MBOX_RD_ST  ( (volatile u32 *) (PBASE + MBOX_OFF + MBOX_RD_ST_OFF) )
#define REG_MBOX_WR_ST  ( (volatile u32 *) (PBASE + MBOX_OFF + MBOX_WR_ST_OFF) )

u32 mbox_get_core_freq();
u32 mbox_get_arm_freq();
u32 mbox_set_arm_freq(u32 freq, u32 disable_turbo);
u32 mbox_get_uart_freq();
u32 mbox_set_uart_freq(u32 uart_freq, u32 disable_turbo);
void mbox_get_firmware_version();
