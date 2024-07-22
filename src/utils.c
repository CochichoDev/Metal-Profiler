#include "utils.h"
#include "api/api.h"

#include <ctype.h>
#include <stdio.h>

int64_t parseNum(STR_P str) {

    while (!isdigit(*str)) {
        if (*str == '\n') return 0;
        str++;
    }

    int64_t num = 0;
    while (isdigit(*str)) {
        num *= 10;
        num += *str - 0x30;
        str++;
    }

    return num;
}

double parseFloat(char *str) {

    while (!isdigit(*str)) {
        if (*str == '\n') return 0;
        str++;
    }

    int64_t num = 0;
    while (isdigit(str)) {
        num *= 10;
        num += *str - 0x30;
        str++;
    }

    return num;
}
