/*
 * Developed by Diogo Cochicho
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <unistd.h>

#include "global.h"
#include "utils.h"
#include "cli.h"

#define DEVICE "/dev/ttyUSB0"

#define MAKEFILE_PATH "Core%d/Debug"

#define FALSE 0
#define TRUE 1

#define USAGE_ERROR() \
    {                                                       \
    perror("Usage: ./autometalbench -i INPUT -o OUTPUT");   \
    exit(1);                                                \
    }

int main(int32_t argc, char **argv) {
    loadAvailableArchs();

    TERM term;
    if (cliInit(&term, STDIN_FILENO, STDOUT_FILENO)) {
        fprintf(stderr, "Error: CLI couldn't be initialized\n");
        return -1;
    }

    cliStart(&term);


    return 0;
}
