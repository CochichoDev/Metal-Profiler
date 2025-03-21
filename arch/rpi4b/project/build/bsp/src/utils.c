#include "ctype.h"

#include "common.h"

size_t itos(s64 num, char *str) {
    if (num == 0) {
        *str++ = '0';
        *str = 0;
        return 1;
    }

    char buf[32];
    size_t idx = 0;

    if (num < 0) {
        buf[idx++] = '-';
    }

    while (num) {
        buf[idx++] = num % 10 + 0x30;
        num /= 10;
    }

    size_t size = idx;
        
    while (idx > 0) {
        *str++ = buf[--idx];
    }

    *str = '\0';
    return size;
}

size_t i16tos(u64 num, char *str) {
    char hex_digits[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    char buf[32];
    size_t idx = 0;
    size_t size = 0;

    if (num == 0) {
        buf[idx++] = '0';
        buf[idx++] = 'x';
        buf[idx++] = '0';
        goto REVERSE;
    }

    while (num) {
        buf[idx++] = hex_digits[num % 16];
        num /= 16;
    }

    buf[idx++] = 'x';
    buf[idx++] = '0';
        
REVERSE:
    size = idx;
    while (idx > 0) {
        *str++ = buf[--idx];
    }

    *str = '\0';
    return size;
}

s32 alllower(s32 c) {
    if (isalpha(c)) c |= 0x20;
    return c;
}

u64 parseUNum(const char *str, u64 len) {
    if (len <= 0) return 0;

    u64 num = 0;

    struct {
        u8 base;
        u8 init;
        const char *init_ptr;
        const char *end_ptr;
        u64 mul;
    } p_info = {10, 0, str, str, 1};

    // First pass
    while (len-- > 0) {
        if (isdigit(*str)) {
            if (p_info.init == 0) {
                if (alllower(*(str+1)) == 'x') {
                    p_info.base = 16;
                    p_info.end_ptr = ++str + 1;
                } else p_info.end_ptr = str;
                p_info.init = 1;
            } 
        } else {
            if (p_info.init == 1 && !isdigit(*str)) {
                p_info.init_ptr = str - 1;
                break;
            }
        }
        ++str;
    }

    if (len == 0) p_info.init_ptr = str - 1;

    while (p_info.init_ptr >= p_info.end_ptr) {
        num += (*p_info.init_ptr - '0') * p_info.mul;
        p_info.mul *= p_info.base;
        p_info.init_ptr--;
    }

    return num;
}

s32 strncmp(const char *s1, const char *s2, size_t len) {
    if ( len <= 0 ) return 0;
    u64 idx = 0;
    while (idx++ < len ) {
        if (*s1 != *s2 || *s1 == 0) return *s1 - *s2; 
        ++s1; ++s2;
    }
    return *(s1-1) - *(s2-1);
}
