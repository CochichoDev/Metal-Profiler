#pragma once

#include "common.h"

/*
#define PM_BASE         (0x02100000U)
#define PBASE           (0xFC000000U)

#define PM_RSTC_OFF     (0x1CU)
#define PM_RSTS_OFF     (0x20U)
#define PM_WDOG_OFF     (0x24U)


#define PM_WDOG_PASSWORD    (0x5a000000U)
#define PM_RSTC_FULLRST     (0x00000020U)

#define REG_PM_RSTC     ( (volatile u32 *) PBASE + PM_BASE + PM_RSTC_OFF )
#define REG_PM_RSTS     ( (volatile u32 *) PBASE + PM_BASE + PM_RSTS_OFF )
#define REG_PM_WDOG     ( (volatile u32 *) PBASE + PM_BASE + PM_WDOG_OFF )
*/

#define PM_RSTC         ((volatile unsigned int*)(0xfe000000+0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(0xfe000000+0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(0xfe000000+0x00100024))
#define PM_WDOG_MAGIC   0x5a000000
#define PM_RSTC_FULLRST 0x00000020

void pm_reset();
