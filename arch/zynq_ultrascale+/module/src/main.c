#include <stdio.h>

#include "utils.h"

void BUILD_PROJECT(CONFIG *config) {
    puts("Makefile INFO");
    callMakefiles(config);
    puts("Info: Binaries compiled");
    puts("*****************************************************");
}

void runBench() {
    
}
