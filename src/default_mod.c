#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "build.h"
#include "types.h"
#include "results.h"
#include "state.h"
#include "tty.h"
#include "t32_i.h"
#include "xsct_i.h"
#include "global.h"
#include "utils.h"
#include "uart.h"
#include "elf_reader.h"

#define DEBUG

static CONFIG *CUR_CFG;

void default_DEPLOY(const char *dest) {
    
}

void default_BUILD_PROJECT(CONFIG *config) {
    CUR_CFG = config;
    puts("Makefile INFO");
    CALL_MAKEFILES(config);
    puts("Info: Binaries compiled");
    puts("*****************************************************");


    const COMP *victim;
    T_UINT iterations;
    if (GET_COMP_BY_ID(config, config->VICTIM_ID, &victim) == -1) {
        fprintf(stderr, "Error: Victim core is not configured");
        return;
    }
    if (GET_PROP_BY_NAME(victim, "ITERATIONS", &iterations) == -1) {
        fprintf(stderr, "Error: ITERATIONS propriety is not configured on the victim core");
        return;
    }
    REGISTER_OUTPUT(T_UINT, iterations, "CYCLES");
    REGISTER_OUTPUT(T_UINT, iterations, "L1D-REFILLS");
    REGISTER_OUTPUT(T_UINT, iterations, "L1D-WB");
    REGISTER_OUTPUT(T_UINT, iterations, "L2-REFILLS");
    REGISTER_OUTPUT(T_UINT, iterations, "L2-WB");
}

void default_INIT_BENCH() {
    /*
    puts("Trace32 INFO");
    pid_t T32_PID = INIT_T32("/opt/t32/bin/pc_linux64/t32marm64-qt");
    if (T32_PID == -1) {
        perror("Error: Could not initialize TRACE32");
        exit(1);
    }
    puts("*****************************************************");
    puts("Info: Trace32 launched");
    puts("*****************************************************");
    */
    
    
    /*
    char script_query[256];
    strcpy(script_query, SELECTED_ARCH.path);
    strcat(script_query, "/project/"XSCTSCRIPT);

    puts("*****************************************************");
    puts("Info: XSCT launched");
    puts("*****************************************************");
    T_INT status = INIT_XSCT(script_query);
    if (status != 0) {
        fprintf(stderr, "Error: Could not initialize XSCT (status: %d)\n", status);
        exit(-1);
    }
    */

    /*
    INIT_TTY(TTY_PORT);
    puts("*****************************************************");
    puts("Info: TTY launched");
    puts("*****************************************************");
    */
    init_uart(-1);
    uart_log();

}

void default_RUN_BENCH(RESULT *results) {
    char script_query[256];
    strcpy(script_query, SELECTED_ARCH.path);
    //strcat(script_query, "/project/"T32SCRIPT);
    //strcat(script_query, "/project/"XSCTSCRIPT);

    //EX_T32_SCRIPT(script_query, SELECTED_ARCH.desc.NUM_CORES, core_state);
    //EX_XSCT_SCRIPT(script_query, SELECTED_ARCH.desc.NUM_CORES, core_state);

    char *run_cmd = "RUN";
    char *info_cmd = "INFO";
    char *load_cmd = "LOAD";
    char *mmu_cmd = "MMU";
    char *reset_cmd = "RESET";
    char *mem_cmd = "MEM";
    char *sys_cmd = "SYS";
    
    char mmutables_binpath[128];
    if (getBinPath(-1, mmutables_binpath) >= 128) {
        fprintf(stderr, "Error: Bin path of bsp application is too big please increase the array size\n");
        exit(1);
    }

    s8 *core_state = calloc(SELECTED_ARCH.desc.NUM_CORES, sizeof(s8));
    size_t num_cores = activeCores(CUR_CFG, core_state);

    if (num_cores != SELECTED_ARCH.desc.NUM_CORES) {
        fprintf(stderr, "Error: The number of available cores does not match with the build.json (expected %d configured %ld)\n", SELECTED_ARCH.desc.NUM_CORES, num_cores);
        exit(1);
    }

    
    #ifdef DEBUG
    printf("Sending MMU ELF...\n");
    #endif
    for (size_t i = 0; i < strlen(mmu_cmd); i++)
        uart_send_byte(mmu_cmd[i]);
    uart_send_byte('\r');
    open_elf(mmutables_binpath, 255 - num_cores);

    #ifdef DEBUG
    printf("MMU ELF sent\n");
    #endif
    uart_log();

    char binpath[128];
    for (size_t core = 0; core < CUR_CFG->NUM-1; core++) {      // CUR_CFG->NUM-1 because the ID 0 is always system wide
        if (core_state[core] != 1) continue;

        #ifdef DEBUG
        printf("Sending ELF of Core %ld...\n", core);
        #endif
        for (size_t i = 0; i < strlen(load_cmd); i++)
            uart_send_byte(load_cmd[i]);
        uart_send_byte('\r');

        if (getBinPath(core, binpath) >= 128) {
            fprintf(stderr, "Error: Bin path of core application is too big please increase the array size\n");
            exit(1);
        }

        open_elf(binpath, core);
        #ifdef DEBUG
        printf("Finished sending ELF of Core %ld\n", core);
        #endif
        uart_log();
    }

    free(core_state);

    for (size_t i = 0; i < strlen(info_cmd); i++)
        uart_send_byte(info_cmd[i]);
    uart_send_byte('\r');

    uart_log();
    sleep(2);

    for (size_t i = 0; i < strlen(run_cmd); i++)
        uart_send_byte(run_cmd[i]);
    uart_send_byte('\r');
    uart_to_result('!', '?', results);
    uart_log();
    
    for (size_t i = 0; i < strlen(reset_cmd); i++)
        uart_send_byte(reset_cmd[i]);
    uart_send_byte('\r');
    uart_log();

}

void default_EXIT_BENCH() {
    //CLOSE_TTY();
    close_uart();
    //CLOSE_T32();
    //CLOSE_XSCT();

    //puts("Info: Trace32 closed");
    //puts("Info: XSCT closed");
}
