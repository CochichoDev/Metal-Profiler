#include "utils.h"

#include "utils.h"

size_t tui_itos(int num, char *str) {
    if (num == 0) {
        *str++ = '0';
        *str = 0;
        return 1;
    }
    char buf[16];
    size_t idx = 0;
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



