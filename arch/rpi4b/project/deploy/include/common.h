#pragma once

#ifndef ASSEMBLY
#include <stdint.h>
#include <stddef.h>

typedef uint8_t     u8;
typedef int32_t     s32;
typedef uint32_t    u32;
typedef uint64_t    s64;
typedef uint64_t    u64;
#endif

#define APBASE      (0xFF800000U)
#define PBASE       (0xFC000000U)

#define STDOUT      UART4
#define BAUDRATE    115200
