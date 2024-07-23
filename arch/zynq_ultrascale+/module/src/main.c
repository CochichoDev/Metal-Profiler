#include <stdio.h>
#include <unistd.h>

#include "api.h"
#include "t32_i.h"
#include "utils.h"

pid_t T32_PID;

void BUILD_PROJECT(CONFIG *config) {
    puts("Makefile INFO");
    callMakefiles(config);
    puts("Info: Binaries compiled");
    puts("*****************************************************");
}

void INIT_BENCH() {
    puts("Trace32 INFO");
    T32_PID = RUN_PROCESS_IMAGE(NULL, "/opt/t32/bin/pc_linux64/t32marm64-qt", "t32marm64-qt", NULL);
    puts("*****************************************************");
    puts("Info: Trace32 launched");
    sleep(1);
    INIT_T32_CONN("localhost", "20000");
}

void RUN_BENCH() {

}

void EXIT_BENCH() {
    CLOSE_T32_CONN();
    puts("Info: Closing Trace32");
    KILL_PROCESS(T32_PID);
}
