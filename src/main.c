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

    /*
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
    */

    return 0;
}
