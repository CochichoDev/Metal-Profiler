#include <stdio.h>

#include "global.h"
#include "utils.h"

CoreConfig *config[NUM_CORES] = { NULL };

void buildProject() {
    puts("Makefile INFO");
    //callMakefiles(config, 1);
    puts("Info: Binaries compiled");
    puts("*****************************************************");
}

void runBench() {
    
}
