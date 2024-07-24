#include "global.h"
#include "utils.h"
#include "api/api.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

void loadAvailableArchs() {
    FILE *archs;

    if (!(archs = fopen(ARCHS_PATH"archs.txt", "r")))
        perror("Error: Couldn't open architecture configuration file");


    char buf[128];
    int i = 0;
    while (fgets(buf, sizeof(buf), archs)) {
        if (!isalnum(buf[0])) continue;
        if (buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = '\0';          // Get rid of the line feed
       
        memcpy(AVAIL_ARCHS.arch[i].name, buf, strlen(buf));
        memcpy(AVAIL_ARCHS.arch[i].path, ARCHS_PATH, strlen(ARCHS_PATH));
        strcat(AVAIL_ARCHS.arch[i].path, buf);
        strcat(AVAIL_ARCHS.arch[i].path, "/module/bin/dmodule.so");
        i++;
    }
    AVAIL_ARCHS.num = i;

    fclose(archs);
}

int64_t parseNum(T_PSTR str) {
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

