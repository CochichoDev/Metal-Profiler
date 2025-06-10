#pragma once

#include "common.h"

size_t itos(s64 num, char *str);
size_t i16tos(u64 num, char *str);

s32 isdigit(s32 c);
u8 ischar(char c);

s32 tolower(s32 c);

u64 parseUNum(const char *str, u64 len);

s32 strncmp(const char *s1, const char *s2, u64 num);
