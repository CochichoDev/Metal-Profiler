#pragma once

#include "common.h"
#include "elf.h"

typedef struct {
    u64 address;
    u64 size;
} ELF_L_SECTION;

err open_elf(const char *path, u8 core_num);

