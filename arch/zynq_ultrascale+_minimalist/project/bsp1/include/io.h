#ifndef IO_H           /* prevent circular inclusions */
#define IO_H           /* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include <stdint.h>

static inline uint8_t Mem_In8(uint64_t Addr)
{
	return *(volatile uint8_t *) Addr;
}

static inline uint16_t Mem_In16(uint64_t Addr)
{
	return *(volatile uint16_t *) Addr;
}

static inline uint32_t Mem_In32(uint64_t Addr)
{
	return *(volatile uint32_t *) Addr;
}

static inline uint64_t Mem_In64(uint64_t Addr)
{
	return *(volatile uint64_t *) Addr;
}

static inline void Mem_Out8(uint64_t Addr, uint8_t Value)
{
	volatile uint8_t *LocalAddr = (volatile uint8_t *)Addr;
	*LocalAddr = Value;
}

static inline void Mem_Out16(uint64_t Addr, uint16_t Value)
{
	volatile uint16_t *LocalAddr = (volatile uint16_t *)Addr;
	*LocalAddr = Value;
}

static inline void Mem_Out32(uint64_t Addr, uint32_t Value)
{
	volatile uint32_t *LocalAddr = (volatile uint32_t *)Addr;
	*LocalAddr = Value;
}

static inline void Mem_Out64(uint64_t Addr, uint64_t Value)
{
	volatile uint64_t *LocalAddr = (volatile uint64_t *)Addr;
	*LocalAddr = Value;
}

#endif
