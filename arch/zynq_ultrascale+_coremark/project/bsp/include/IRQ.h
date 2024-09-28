#ifndef IRQ_H
#define IRQ_H

#if !defined(INIT_BUDGET)
#define INIT_BUDGET 1e4
#endif
#if !defined(PERIOD)
#define PERIOD 10
#endif
#if !defined(REPLENISHMENT)
#define REPLENISHMENT 3e4
#endif


void irq_handler();

#endif
