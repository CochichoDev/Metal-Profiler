#pragma once

#include "common.h"

#define GIC_BASE            ((APBASE) + 0x40000U)
#define ARMC_BASE           ((PBASE) + 0x200B000U)

#define GICD_BASE           (0x1000U)
#define GICC_BASE           (0x2000U)

#define IRQn_SET_EN_2_OFF(CORE)     (0x218U + (0x40U * CORE))
#define IRQn_CLR_EN_2_OFF(CORE)     (0x228U + (0x40U * CORE))
#define IRQ_STATUS2_OFF     (0x238U)
#define SWIRQ_SET_OFF       (0x3F0U)
#define SWIRQ_CLEAR_OFF     (0x3F4U)

#define GICD_CTRL_OFF       (0x0U)
#define GICD_ISENABLER0_OFF (0x100U)
#define GICD_ISENABLER1_OFF (0x104U)
#define GICD_ISENABLER2_OFF (0x108U)
#define GICD_ISPENDR0_OFF   (0x200U)
#define GICD_ISPENDR1_OFF   (0x204U)
#define GICD_ISPENDR2_OFF   (0x208U)
#define GICD_ITARGETSR_OFF  (0x800U)
#define GICD_SGIR_OFF       (0xF00U)

#define GICC_CTRL_OFF       (0x0U)
#define GICC_PMR_OFF        (0x4U)
#define GICC_IAR_OFF        (0xCU)
#define GICC_EOIR_OFF       (0x10U)
#define GICC_RPR_OFF        (0x14U)
#define GICC_HPPIR_OFF      (0x18U)
#define GICC_AIAR_OFF       (0x20U)
#define GICC_AEOIR_OFF      (0x24U)


#define REG_IRQn_SET_EN_2(CORE)       ((volatile u32 *) (ARMC_BASE + IRQn_SET_EN_2_OFF(CORE)) )
#define REG_IRQn_CLR_EN_2(CORE)       ((volatile u32 *) (ARMC_BASE + IRQn_CLR_EN_2_OFF(CORE)) )
#define REG_SWIRQ_SET       ((volatile u32 *) (ARMC_BASE + SWIRQ_SET_OFF) )
#define REG_SWIRQ_CLEAR     ((volatile u32 *) (ARMC_BASE + SWIRQ_CLEAR_OFF) )

#define REG_GICD_CTRL       ((volatile u32 *) (GIC_BASE + GICD_BASE + GICD_CTRL_OFF) )
#define REG_GICD_ISENABLER0 ((volatile u32 *) (GIC_BASE + GICD_BASE + GICD_ISENABLER0_OFF) )
#define REG_GICD_ISENABLER1 ((volatile u32 *) (GIC_BASE + GICD_BASE + GICD_ISENABLER1_OFF) )
#define REG_GICD_ISENABLER2 ((volatile u32 *) (GIC_BASE + GICD_BASE + GICD_ISENABLER2_OFF) )
#define REG_GICD_ITARGETSR(index)   ((volatile u32 *) (GIC_BASE + GICD_BASE + GICD_ITARGETSR_OFF + ((index / 4) * 0x4)) )
#define SHIFT_GICD_ITARGETSR(index) ((index % 4) * 0x8)
#define REG_GICD_SGIR       ((volatile u32 *) (GIC_BASE + GICD_BASE + GICD_SGIR_OFF) )

#define REG_GICC_CTRL       ((volatile u32 *) (GIC_BASE + GICC_BASE + GICC_CTRL_OFF) )
#define REG_GICC_PMR        ((volatile u32 *) (GIC_BASE + GICC_BASE + GICC_PMR_OFF) )
#define REG_GICC_IAR        ((volatile u32 *) (GIC_BASE + GICC_BASE + GICC_IAR_OFF) )
#define REG_GICC_EIOR       ((volatile u32 *) (GIC_BASE + GICC_BASE + GICC_EOIR_OFF) )
