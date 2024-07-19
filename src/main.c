/*
* Developed by Diogo Cochicho
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "tty.h"
#include "processHandler.h"
#include "global.h"
#include "bench.h"
#include "calc.h"
#include "plot.h"
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

void callMakefiles(CoreConfig **config, uint8_t cacheColoring);
    
int main(int32_t argc, char **argv) {
    initializeFramework();

    TERM term;
    if (cliInit(&term, STDIN_FILENO, STDOUT_FILENO)) {
        fprintf(stderr, "Error: CLI couldn't be initialized\n");
        return -1;
    }

    cliStart(&term);

    ttyFD tty;
    CoreConfig *config[NUM_CORES] = { NULL };
    uint8_t cacheColoring = 0;

    if (argc < 5) USAGE_ERROR();     
    const char *output = NULL, *input = NULL;

    for (uint32_t arg_num = 1 ; arg_num < argc ; arg_num++) {
        if (argv[arg_num][0] == '-') {
            if (argv[arg_num][2] != '\0') USAGE_ERROR();    // This ensures that the option is only one character long
            switch (argv[arg_num][1]) {
                case 'i':
                    if (arg_num + 1 >= argc) USAGE_ERROR();         // In case there is no argument after the desired option
                    input = argv[arg_num+1];
                    break;
                case 'o':
                    if (arg_num + 1 >= argc) USAGE_ERROR();         // In case there is no argument after the desired option
                    output = argv[arg_num+1];
                    break;
                case 'c':
                    cacheColoring = 1;
                    break;
                default:
                    USAGE_ERROR();
            }
        }
    }

    getInitialConfig(input, config);

    puts("Makefile INFO");
    callMakefiles(config, cacheColoring);
    puts("Info: Binaries compiled");
    puts("*****************************************************");
    
    puts("Trace32 INFO");
    pid_t t32 = launchProcess("/opt/t32/bin/pc_linux64/t32marm64-qt", "t32marm64-qt", NULL);
    puts("*****************************************************");
    puts("Info: Trace32 launched");
    sleep(1);
    initializeTrace32Conn("localhost", "20000");

    puts("Info: Starting tty");
    tty = openUltrascaleTTY(DEVICE);

    RESULTS(uint64_t) fullConfigResults;
    RESULTS(uint64_t) isoConfigResults;
    initializeResults(uint64_t, &fullConfigResults, REP_EXP, "All_Cores");
    initializeResults(uint64_t, &isoConfigResults, REP_EXP, "Isolation");

    for (uint8_t idx = 0 ; idx < REP_EXP ; idx++) {
        fullConfigResults.cycles[idx] = benchFullConfig(config, tty);
        isoConfigResults.cycles[idx] = benchIsolationConfig(config, tty);
    }

    calculateMetrics(&isoConfigResults);
    calculateMetrics(&fullConfigResults);
    saveData(uint64_t, output, &isoConfigResults, &fullConfigResults, NULL);
    
    RESULTS(double) degradation;
    initializeResults(double, &degradation, REP_EXP, "Degradation");
    calculateDegradation(&degradation, &isoConfigResults, &fullConfigResults);
    char output_deg[128] = "\0";
    strcpy(output_deg, output);
    strcat(output_deg, "_deg");
    saveData(double, output_deg, &degradation, NULL);

    plotBarWErrors(output);
    plotBarWErrors(output_deg);
    
    destroyResults(uint64_t, &fullConfigResults);
    destroyResults(uint64_t, &isoConfigResults);
    destroyResults(double, &degradation);

    closeTrace32Conn();
    puts("Info: Closing Trace32");
    killProcess(t32);

    closeUltrascaleTTY(tty);
    puts("Info: Closing tty");

    for (uint8_t i = 0 ; i < NUM_CORES ; i++)
        free(config[i]);

    return 0;
}

void callMakefiles(CoreConfig **config, uint8_t cacheColoring) {
    //pid_t make_META_pids[NUM_CORES] = { 0 };
    pid_t make_pids[NUM_CORES] = { 0 };
    pid_t make_standalone;
    if (cacheColoring) {
        make_standalone = launchProcess("/bin/make", "make", "-C", "Meta/psu_cortexa53_0/standalone_domain/bsp/psu_cortexa53_0/libsrc/standalone_v7_6/src", "EXTRA_COMPILER_FLAGS=-DCacheColoring", NULL);
    } else {
        make_standalone = launchProcess("/bin/make", "make", "-C", "Meta/psu_cortexa53_0/standalone_domain/bsp/psu_cortexa53_0/libsrc/standalone_v7_6/src", NULL);
    }
    waitpid(make_standalone, NULL, 0);
    /*
    make_META_pids[0] = launchProcess("/bin/make", "make", "-C", "Meta/psu_cortexa53_0/standalone_domain/bsp", NULL);
    make_META_pids[1] = launchProcess("/bin/make", "make", "-C", "Meta/psu_cortexa53_0/standalone_domain/bsp", NULL);
    make_META_pids[2] = launchProcess("/bin/make", "make", "-C", "Meta/psu_cortexa53_0/standalone_domain/bsp", NULL);
    make_META_pids[3] = launchProcess("/bin/make", "make", "-C", "Meta/psu_cortexa53_0/standalone_domain/bsp", NULL);
    for (size_t i = 0 ; i < NUM_CORES ; i++) {
        waitpid(make_META_pids[i], NULL, 0);
    }
    */

    for (uint8_t i = 0 ; i < NUM_CORES ; i++) {
        if (config[i] != NULL) {
            const char *CFLAGS = makeString(config[i]);
            puts(CFLAGS);
            char PATH[64];
            sprintf(PATH, MAKEFILE_PATH, i);
            make_pids[i] = launchProcess("/bin/make", "make", "-C", PATH, "clean", "all", CFLAGS, NULL);
            free((void *) CFLAGS);
        }
    }
    for (uint8_t i = 0 ; i < NUM_CORES ; i++) {
        if (make_pids[i]) {
            waitpid(make_pids[i], NULL, 0);
        }
    }
}
