
/*--------------------------- PMH.h -------------------------------------
 |  File PMH.h
 |
 |  Description: This file provides us with functions for managing the
 |  ARM Performance Monitor Hardware
 |  Version: V1
 *-----------------------------------------------------------------------*/

 
#define PMSELR_MASK  0xFFFFFFE0
#define PMXEVTYPER_MASK  0xFFFFFF00
#define PMCNTENSET_MASK  0x7FFFFFC0
#define PMCR_MASK  0xFFFFFFC0
#define PMCNTENCLR_MASK  0x7FFFFFC0
#define PMOVSR_MASK  0x7FFFFFC0
#define CPUACTLR_MASK	0xF9FFFFFF
#define CPUACTLR_MASK2	0xE7FFFFFF




 /* select_evt_counter
 *
 * Description: The counter used in the coprocessor is selected, without modifying the reserved bits.
 *
 * Parameter:
 * 				- int counter_id_f: Indicates which counter is selected (between 0 and 5 in ARM 15)
 *
 * Returns:		Nothing
 *
 * */
static inline void select_evt_counter(int counter_id_f)  {
	  register u64 value=0;
     __asm__ __volatile("MRS %0, PMSELR_EL0" : "=r"(value));

     __asm__ __volatile("MSR PMSELR_EL0, %0" :: "r"((value&PMSELR_MASK)|counter_id_f));

}


/* event_track
 *
 * Description: The event type is selected, without modifying the reserved bits.
 *
 * Parameter:
 * 				- int event_id_f: Indicates which event is selected (between 0x00 and 0x7E in ARM 15)
 *
 * Returns:		Nothing
 *
 * */
static inline void event_track(int event_id_f)  {
     u64 value=0;
      __asm__ __volatile("MRS %0, PMXEVTYPER_EL0" : "=r"(value));
      __asm__ __volatile("MSR PMXEVTYPER_EL0, %0" :: "r"((value&PMXEVTYPER_MASK)|event_id_f));

}


/* enable_evt_counter
 *
 * Description: Enables the counters to be used, without modifying the reserved bits.
 *
 * Parameter:
 * 				- int counter_id_f: Indicates which counter you want to enable (between 0 and 5 in ARM 15)
 *
 * Returns:		Nothing
 *
 * */
static inline void enable_evt_counter(int counter_id_f)  {
	u64 value=0;
   __asm__ __volatile("MRS %0, PMCNTENSET_EL0" : "=r" (value));
   __asm__ __volatile("MSR PMCNTENSET_EL0, %0" :: "r" ((value & PMCNTENSET_MASK) | 1<<counter_id_f)); // 0x80000000 = Cycle counter enable set

}


/* enable_evt_counters
 *
 * Description: Enables the counters to be used, without modifying the reserved bits.
 *
 * Parameter:
 * 				- int counter_id_f: Indicates which counters you want to enable (between 0x0 and 03F in ARM cortex a15)
 *
 * Returns:		Nothing
 *
 * */
static inline void enable_evt_counters(int counter_id_f)  {
	 u64 value=0;
   __asm__ __volatile("MRS %0, PMCNTENSET_EL0" : "=r" (value));
   __asm__ __volatile("MSR PMCNTENSET_EL0, %0" :: "r" ((value & PMCNTENSET_MASK) | counter_id_f)); // 0x80000000 = Cycle counter enable set

}
 

/* enable_all_counters
 *
 * Description: Enables the cycle and event counters to be used, without modifying the reserved bits.
 *
 * Parameter:
 * 				- int counter_id_f: Indicates which EVT counters you want to enable (between 0x0 and 0x3F in ARM cortex a15)
 *
 * Returns:		Nothing
 *
 * */
static inline void enable_all_counters(int counter_id_f)  {
	 u64 value=0;
 
   asm volatile("mrs  %0, PMCNTENSET_EL0" : "=r" (value));
   asm volatile("msr PMCNTENSET_EL0, %0" :: "r" ((value & PMCNTENSET_MASK) | (counter_id_f) |(0x80000000)));

}



/* disable_counters
 *
 * Description: Disables all the counters leaving just the reserved bits.
 *
 * Parameter:
 * 				- int counter_id_f: Indicates which EVT counters you want to enable (between 0x0 and 0x3F in ARM cortex a15)
 *
 * Returns:		Nothing
 *
 * */
static inline void disable_counters(int counter_id_f) {
  asm volatile("msr PMCNTENCLR_EL0, %0" :: "r" (PMCNTENCLR_MASK | counter_id_f));

}

/* disable_all_counters
 *
 * Description: Disables all the counters leaving just the reserved bits.
 *
 * Parameter:
 * 				- int counter_id_f: Indicates which EVT counters you want to enable (between 0x0 and 0x3F in ARM cortex a15)
 *
 * Returns:		Nothing
 *
 * */
static inline void disable_all_counters(int counter_id_f) {
   asm volatile("MSR PMCNTENCLR_EL0, %0" ::  "r" (PMCNTENCLR_MASK | counter_id_f | (0x80000000))); // 0x80000000 = Cycle counter disable set

}





/* reset_evt_counter
 *
 * Description: Enables all counters and resets the general counters as well as the cycle counter and setting its count divider, without modifying the reserved bits.
 *
 * Parameter:
 * 				- int enable_divider: Indicates if cycle count divider is used(0 or 1)
 *
 * Returns:		Nothing
 *
 * */
static inline void reset_evt_counter(unsigned int enable_divider) {
     u64 value=0;
     u64 temp=0;
	temp = (1 | 2);
	if (enable_divider)
		temp |= 8;
   __asm__ __volatile("MRS %0, PMCR_EL0" : "=r" (value));
   __asm__ __volatile("MSR PMCR_EL0, %0" :: "r" ((value & PMCR_MASK) | temp));
}

/* reset_all_counter
 *
 * Description: Enables and resets the general counters as well as the selected cycle counter and setting its count divider (if required), without modifying the reserved bits.
 *
 * Parameter:
 * 				- int enable_divider: Indicates if cycle count divider is used(0 or 1)
 *
 * Returns:		Nothing
 *
 * */
static inline void reset_all_counters(unsigned int enable_divider) {
	register u64 value=0;
    register u64 temp=0;
	temp = (1 | 2 | 4);
	if (enable_divider)
		temp |= 8;
   __asm__ __volatile("MRS %0, PMCR_EL0"  : "=r" (value));
   __asm__ __volatile("MSR PMCR_EL0, %0"  :: "r" ((value & PMCR_MASK) | temp));

}


/* enable_counter_divider
 *
 * Description: Enables the clock divider. PMCCNTR counts every clock cycle.
 *
 * Parameter:
 *              None
 *
 * Returns:     Nothing
 *
 * */
static inline void enable_counter_divider() {
    u64 value=0;
    u64 temp=0;

    temp = 8;
   __asm__ __volatile("MRS %0, PMCR_EL0"  : "=r" (value));
   __asm__ __volatile("MSR PMCR_EL0, %0" :: "r" ((value & PMCR_MASK) | temp));

}

/* read_evt_counter
 *
 * Description: Reads the event counter register (Performance Monitor Count Registers).
 *
 * Parameter:
 *              None
 *
 *
 * Returns:		The event value is returned
 *
 * */
static inline u64 read_evt_counter()  {
   register u64 value=0;
   __asm__ __volatile("MRS %0,  PMXEVCNTR_EL0\t\n": "=r"(value));

   return value;
}



/* enable_cycle_counter
 *
 * Description: Enables only the cycle counter, without modifying the reserved bits.
 *
 * Parameter:
 * 				None
 *
 * Returns:		Nothing
 *
 * */
static inline void enable_cycle_counter()  {
	 u64 value=0;

   __asm__ __volatile("mrs  %0, PMCNTENSET_EL0" : "=r" (value));
   __asm__ __volatile("msr PMCNTENSET_EL0, %0" :: "r" ((value & PMCNTENSET_MASK) |(0x80000000)));

 
}




/* reset_cycle_counter
 *
 * Description: Enables all counters as well as reseting cycle counter value and setting its count divider, without modifying the reserved bits.
 *
 * Parameter:
 * 				- int enable_divider: Indicates if cycle count divider is used(0 or 1)
 *
 * Returns:		Nothing
 *
 * */
static inline void reset_cycle_counter(unsigned int enable_divider) {
    u64 value=0;
    u64 temp=0;
	temp = 1|4 ;
	if (enable_divider)
		temp |= 8;
     asm volatile("mrs  %0, PMCR_EL0" : "=r" (value));
     asm volatile("msr PMCR_EL0, %0" :: "r" ((value & PMCR_MASK) | temp));

}



/* read_cycle_counter
 *
 * Description: Reads the Cycle Counter Register (CCNT).
 *
 * Parameter:   None
 *
 *
 * Returns:		The cycle counter value is returned
 *
 * */
static inline   u64 read_cycle_counter()  {
  u64 value=0;
   asm volatile("mrs %0, pmccntr_el0" : "=r" (value));

   //asm volatile("mrs %0, pmcr_el0" : "=r" (val)); 
   return value;
}




/* read_counters_overflow
 *
 * Description: Reads the Performance Monitor Overflow Flag Status Register (PMSOVSR ).
 *
 * Parameter:   None
 *
 *
 * Returns:     The counters overflow flags are returned
 *
 * */
static inline u64 read_counters_overflow()  {
   u64 value=0;
   __asm__ __volatile("MRS %0, PMOVSCLR_EL0": "=r"(value));
   return (value&0x8000003F); // MSB = Counter cycle overflow
}

/* clear_overflows
 *
 * Description: Clears all counters overflows flags (MSB = Counter cycle overflow)
 *
 * Parameter:   None
 *
 *
 * Returns:		Nothing
 *
 * */
static inline void clear_overflows()  {
    u64  value=0;

	__asm__ __volatile("MRS %0, PMOVSCLR_EL0" : "=r"(value));
	__asm__ __volatile("MSR  PMOVSCLR_EL0,  %0" :: "r"((value & PMOVSR_MASK) | 0x8000003F));
}


/* reset_cycle_counter
 *
 * Description: Changes the configuration on the no allocate functionality of BIU regarding L1 Cache
 *
 * Parameter:
 * 				- u8 mode: Selects the mode of configuration of this functionality:
 * 						1. 0b00: Default value, no allocate every 4th consecutive cache line
 * 						2. 0b01: no allocate the 64th consecutive cache line write miss
 * 						3. 0b10: no allocate the 128th consecutive cache line write miss
 * 						4. 0b11: disables the no allocate feature
 *
 * Returns:		Nothing
 *
 */
static inline void no_allocate_threshold_L1(u32 mode) {
	register u64 value = 0;

	__asm__ __volatile__("MRS %0, S3_1_C15_C2_0" : "=r"(value));
	__asm__ __volatile__("MSR S3_1_C15_C2_0, %0" : : "r" ((value & CPUACTLR_MASK) | (mode << 25)));

}

/* reset_cycle_counter
 *
 * Description: Changes the configuration on the no allocate functionality of BIU regarding L2 Cache
 *
 * Parameter:
 * 				- u8 mode: Selects the mode of configuration of this functionality:
 * 						1. 0b00: Default value, no allocate every 4th consecutive cache line
 * 						2. 0b01: no allocate the 64th consecutive cache line write miss
 * 						3. 0b10: no allocate the 128th consecutive cache line write miss
 * 						4. 0b11: disables the no allocate feature
 *
 * Returns:		Nothing
 *
 */
static inline void no_allocate_threshold_L2(u32 mode) {
	register u64 value = 0;

	__asm__ __volatile__("MRS %0, S3_1_C15_C2_0" : "=r"(value));
	__asm__ __volatile__("MSR S3_1_C15_C2_0, %0" : : "r" ((value & CPUACTLR_MASK2) | (mode << 27)));

}



