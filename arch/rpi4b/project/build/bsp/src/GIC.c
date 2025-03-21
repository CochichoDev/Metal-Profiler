#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "GIC.h"


void gicd_enable_int(irq_no irq) {
    size_t reg_n = (irq / GIC_GICD_ISENABLER_PER_REG);

    // Since there is a need to make a shift of 2 bits for every new icfgr 
    size_t shift = (irq % GIC_GICD_ISENABLER_PER_REG);

    *REG_GIC_GICD_ISENABLER(reg_n) |= (0x1U << shift);
}
void gicd_disable_int(irq_no irq) {
    size_t reg_n = (irq / GIC_GICD_ICENABLER_PER_REG);

    // Since there is a need to make a shift of 2 bits for every new icfgr 
    size_t shift = (irq % GIC_GICD_ICENABLER_PER_REG);

    *REG_GIC_GICD_ICENABLER(reg_n) |= (0x1U << shift);
}

uint8_t gicd_is_irq_pending(irq_no irq) {
    size_t reg_n = (irq / GIC_GICD_ICPENDR_PER_REG);

    // Since there is a need to make a shift of 2 bits for every new icfgr 
    size_t shift = (irq % GIC_GICD_ICPENDR_PER_REG);

    return ( (*REG_GIC_GICD_ICPENDR(reg_n) >> shift) & 0x1U);
}

void gicd_clear_pending(irq_no irq) {
    size_t reg_n = (irq / GIC_GICD_ICPENDR_PER_REG);

    // Since there is a need to make a shift of 2 bits for every new icfgr 
    size_t shift = (irq % GIC_GICD_ICPENDR_PER_REG);

    *REG_GIC_GICD_ICPENDR(reg_n) |= (0x1U << shift);
}

void gicd_set_target(irq_no irq, uint32_t target) {
    size_t reg_n = (irq / GIC_GICD_ITARGETSR_PER_REG);

    // Since there is a need to make a shift of 2 bits for every new icfgr 
    size_t shift = (irq % GIC_GICD_ITARGETSR_PER_REG) * GIC_GICD_ITARGETSR_SIZE_PER_REG;

    uint32_t itarget_value = *REG_GIC_GICD_ITARGETSR(reg_n);

    itarget_value &= ~(((uint32_t) 0xFFU) << shift);
    itarget_value |= (((uint32_t) target) << shift);

    *REG_GIC_GICD_ITARGETSR(reg_n) = itarget_value;
}

void gicd_set_priority(irq_no irq, uint32_t priority) {
    size_t reg_n = (irq / GIC_GICD_IPRIORITY_PER_REG);

    // Since there is a need to make a shift of 2 bits for every new icfgr 
    size_t shift = (irq % GIC_GICD_IPRIORITY_PER_REG) * GIC_GICD_IPRIORITY_SIZE_PER_REG;

    uint32_t iprio_value = *REG_GIC_GICD_IPRIORITYR(reg_n);

    iprio_value &= ~(((uint32_t) 0xFFU) << shift);
    iprio_value |= (((uint32_t) priority) << shift);

    *REG_GIC_GICD_IPRIORITYR(reg_n) = iprio_value;
}

void gicd_config(irq_no irq, uint32_t config) {
    size_t reg_n = irq / GIC_GICD_ICFGR_PER_REG;

    // Since there is a need to make a shift of 2 bits for every new icfgr 
    size_t shift = (irq % GIC_GICD_ICFGR_PER_REG) * GIC_GICD_ICFGR_SIZE_PER_REG;

    uint32_t icfgr_value = *REG_GIC_GICD_ICFGR(reg_n);

    icfgr_value &= ~(((uint32_t) 0x3U) << shift);
    icfgr_value |= (((uint32_t) config) << shift);

    *REG_GIC_GICD_ICFGR(reg_n) = icfgr_value;
}

void disable_gicd() {
    // Disable distributor
    *REG_GIC_GICD_CTLR = GIC_GICD_CTLR_DISABLE;
}

void enable_gicd() {
    // Disable distributor
    *REG_GIC_GICD_CTLR = GIC_GICD_CTLR_ENABLE;
}

irq_no gicc_get_pending() {
    return (*REG_GIC_GICC_IAR & GICC_IAR_INTR_IDMASK);
}

uint64_t gicc_clear_pending() {
    uint64_t num_pending = 0;
    for (uint32_t iar_value = *REG_GIC_GICC_IAR ; (iar_value & GICC_IAR_INTR_IDMASK) != GICC_IAR_SPURIOUS_INTR ; \
            iar_value = *REG_GIC_GICC_IAR) {
        *REG_GIC_GICC_EOIR = iar_value;
        num_pending++;
    }
    return num_pending;
}

void initGICC() { 
    uart_str("Initializing GICC"); uart_nl();
    
    *REG_GIC_GICC_CTLR = GICC_CTLR_DISABLE;

    // Set the priority threshold of the CPU to the lowest
    // higher values correspond to lower priority
    *REG_GIC_GICC_PMR = GICC_PMR_PRIO_MIN;

    // The IRQs should all be in one group
    *REG_GIC_GICC_BPR = GICC_BPR_NO_GROUP;

    // Remove all pending IRQs
    gicc_clear_pending();

    *REG_GIC_GICC_CTLR = GICC_CTLR_ENABLE;
}

void initGICD() {
    uart_str("Initializing GICD"); uart_nl();
    /* 
     * In order to configure distributor and interuptins
     * they first need to be disabled
     */
    volatile uint32_t GIC_INT_MAX = (((*REG_GIC_GICD_TYPER) & 0x1Fu) + 1) * 32;

    // Disable distributor
    *REG_GIC_GICD_CTLR = GIC_GICD_CTLR_DISABLE;

    size_t n_regs = GIC_INT_MAX / GIC_GICD_INT_PER_REG;

    // Disable all CPU private IRQs
    for (size_t reg_idx = 0 ; reg_idx < n_regs ; reg_idx++) {
        *REG_GIC_GICD_ICENABLER(reg_idx) = ~((uint32_t) 0);
    }

    // Clear all pending CPU private IRQs
    for (size_t reg_idx = 0 ; reg_idx < n_regs ; reg_idx++) {
        *REG_GIC_GICD_ICPENDR(reg_idx) = ~((uint32_t) 0);
    }

    // Clear all pending CPU private IRQs
    n_regs = GIC_INT_MAX / GIC_GICD_IPRIORITY_PER_REG;
    for (size_t reg_idx = 0 ; reg_idx < n_regs ; reg_idx++) {
        *REG_GIC_GICD_IPRIORITYR(reg_idx) = ~((uint32_t) 0);
    }
    
    // Set target processor for shared peripherals IRQs as processor #0
    n_regs = GIC_INT_MAX / GIC_GICD_ITARGETSR_PER_REG;
    for (size_t reg_idx = GIC_GICD_INTNO_SPI0/GIC_GICD_ITARGETSR_PER_REG ; \
            reg_idx < n_regs ; reg_idx++) {
        *REG_GIC_GICD_ITARGETSR(reg_idx) = 0x01010101;
    }

    // Set type of all peripheral IRQs to level triggered
    n_regs = GIC_INT_MAX / GIC_GICD_ICFGR_PER_REG;
    for (size_t reg_idx = GIC_GICD_INTNO_PPI0/GIC_GICD_ITARGETSR_PER_REG ; \
            reg_idx < n_regs ; reg_idx++) {
        *REG_GIC_GICD_ICFGR(reg_idx) = GIC_GICD_ICFGR_LEVEL;
    }

    *REG_GIC_GICD_CTLR = GIC_GICD_CTLR_ENABLE;
}

void init_irq() {
    initGICD();
    initGICC();

    gicd_config(PTIMER_ID, GIC_GICD_ICFGR_EDGE);
    gicd_config(VTIMER_ID, GIC_GICD_ICFGR_EDGE);
//#ifdef CORE1
    gicd_config(PM1_ID, GIC_GICD_ICFGR_EDGE);
//#endif
//#ifdef CORE2
    gicd_config(PM2_ID, GIC_GICD_ICFGR_EDGE);
//#endif
//#ifdef CORE3
    gicd_config(PM3_ID, GIC_GICD_ICFGR_EDGE);
//#endif
//#ifdef CORE4
    gicd_config(PM4_ID, GIC_GICD_ICFGR_EDGE);
//#endif
    // Set Virtual timer ITQ with highest priority
    gicd_set_priority(PTIMER_ID, 0);
    gicd_set_priority(VTIMER_ID, 0);
//#ifdef CORE1
    gicd_set_priority(PM1_ID, 0);
//#endif
//#ifdef CORE2
    gicd_set_priority(PM2_ID, 0);
//#endif
//#ifdef CORE3
    gicd_set_priority(PM3_ID, 0);
//endif
//ifdef CORE4
    gicd_set_priority(PM4_ID, 0);
//#endif
    // Since VTIMER is a PPI it doesn't really matter the target
    gicd_set_target(PTIMER_ID, 0x1U);
    gicd_set_target(VTIMER_ID, 0x1U);
//#ifdef CORE1
    gicd_set_target(PM1_ID, 0x1U);
//endif
//ifdef CORE2
    gicd_set_target(PM2_ID, 0x2U);
//endif
//ifdef CORE3
    gicd_set_target(PM3_ID, 0x4U);
//endif
//ifdef CORE4
    gicd_set_target(PM4_ID, 0x8U);
//#endif

    gicd_clear_pending(PTIMER_ID);
    gicd_clear_pending(VTIMER_ID);
//#ifdef CORE1
    gicd_clear_pending(PM1_ID);
//endif
//ifdef CORE2
    gicd_clear_pending(PM2_ID);
//endif
//ifdef CORE3
    gicd_clear_pending(PM3_ID);
//#endif
//#ifdef CORE4
    gicd_clear_pending(PM4_ID);
//#endif

    gicd_enable_int(VTIMER_ID);
    gicd_enable_int(PTIMER_ID);
//#ifdef CORE1
    gicd_enable_int(PM1_ID);
//#endif
//#ifdef CORE2
    gicd_enable_int(PM2_ID);
//#endif
//#ifdef CORE3
    gicd_enable_int(PM3_ID);
//#endif
//#ifdef CORE4
    gicd_enable_int(PM4_ID);
//#endif
}

void stop_irq() {
    disable_gicd();
    gicd_disable_int(PTIMER_ID);
//#ifdef CORE1
    gicd_disable_int(PM1_ID);
//#endif
//#ifdef CORE2
    gicd_disable_int(PM2_ID);
//#endif
//#ifdef CORE3
    gicd_disable_int(PM3_ID);
//#endif
//#ifdef CORE4
    gicd_disable_int(PM4_ID);
//#endif
}
