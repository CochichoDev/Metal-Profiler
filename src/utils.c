#include "global.h"
#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

T_PSTR getNameFromPath(T_PSTR path) {
    // Get the name of the executable
    T_PSTR slash_marker = path;
    T_PSTR last_slash = path;
    while (*slash_marker != '\0') {
        if (*slash_marker == '/')
            last_slash = slash_marker;
        slash_marker++;
    }
    last_slash++;

    return last_slash;
}

void loadAvailableArchs() {
    FILE *archs;

    if (!(archs = fopen(ARCHS_PATH ARCH_CONFIG, "r")))
        perror("Error: Couldn't open architecture configuration file");


    char buf[128];
    int i = 0;
    while (fgets(buf, sizeof(buf), archs)) {
        if (!isalnum(buf[0])) continue;
        if (buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = '\0';          // Get rid of the line feed
       
        memcpy(AVAIL_ARCHS.arch[i].name, buf, strlen(buf)+1);
        memcpy(AVAIL_ARCHS.arch[i].path, ARCHS_PATH, strlen(ARCHS_PATH)+1);
        strcat(AVAIL_ARCHS.arch[i].path, buf);
        strcat(AVAIL_ARCHS.arch[i].path, "/module/bin/dmodule.so");
        i++;
    }
    AVAIL_ARCHS.num = i;

    fclose(archs);
}

void loadAvailableConfigs() {
    FILE *configs;

    
    char config_list_path[256];

    memcpy(config_list_path, ARCHS_PATH, strlen(ARCHS_PATH)+1);
    strcat(config_list_path, SELECTED_ARCH.name);
    strcat(config_list_path, "/configs.list");


    if (!(configs = fopen(config_list_path, "r")))
        perror("Error: Couldn't open configuration list file");

    char buf[128];
    int i = 0;
    while (fgets(buf, sizeof(buf), configs)) {
        if (!isnotblank(buf[0])) continue;
        if (buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = '\0';          // Get rid of the line feed
       
        char *name = getNameFromPath(buf);
        memcpy(AVAIL_CONFIGS.config[i].name, name, strlen(name)+1);
        memcpy(AVAIL_CONFIGS.config[i].path, ARCHS_PATH, strlen(ARCHS_PATH)+1);
        strcat(AVAIL_CONFIGS.config[i].path, SELECTED_ARCH.name);
        strcat(AVAIL_CONFIGS.config[i].path, "/");
        strcat(AVAIL_CONFIGS.config[i].path, buf);
        i++;
    }
    AVAIL_CONFIGS.num = i;

    fclose(configs);
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

