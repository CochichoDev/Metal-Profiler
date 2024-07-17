/*
 * Developed by Diogo Cochicho
 */
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "processHandler.h"
#include "t32.h"

/*
 * The variadic arguments need to finish with a null pointer
 */
pid_t launchProcess(const char *path, ...) {
    va_list va;
    const char *args[16];

    va_start(va, path);
    uint8_t idx;
    for (idx = 0; idx < sizeof(args)/sizeof(char*); idx++) {
        const char *arg = va_arg(va, char*);
        if (!arg) break;
        args[idx] = arg;
    }
    // The execv function needs to be ended with a NULL pointer
    args[idx] = NULL;
    va_end(va);


    pid_t child_process = fork();

    /*
     * In case the current process is the child
     * change the process image to the one demanded
     */
    if (child_process == -1) { 
        perror("Error: Could not fork the current process");
        exit(1);
    }
    if (!child_process) {
        //close(STDOUT_FILENO);
        execv(path, (char**) args);
    }

    return child_process;
}

void killProcess(pid_t process) {
    kill(process, SIGTERM);
    if (kill<0) {
        perror("Error: Could not close child process");
        exit(1);
    }
}

void initializeTrace32Conn(const char *node, const char *port) {
    if (T32_Config("NODE=", node) != T32_OK) {
        perror("Error: Could not configure node on T32 connection\n");
        exit(1);
    }
    if (T32_Config("PORT=", port) != T32_OK) {
        perror("Error: Could not configure port on T32 connection\n");
        exit(1);
    }

    while (1) {
        if (T32_Init() == T32_OK) {
            if (T32_Attach(T32_DEV_ICD) == T32_OK) {
                printf("Info: Connection to Trace32 established\n");
                break;
            }
        }
        fprintf(stderr, "Error: Could not initilize T32 connection\n");
        fprintf(stderr, "Info: Retrying connection...\n");
        sleep(5);
    }

    puts("All T32 connection issues were sucessful");
}

void closeTrace32Conn() {
    if (T32_Exit() != T32_OK) {
        perror("Error: Could not close T32 connection\n");
        exit(1);
    }
}

void executeTrace32Script(const char *scriptname) {
    if (T32_Cmd_f("DO %s", scriptname) != T32_OK) {
        fprintf(stderr, "Error: Could not execute %s\n", scriptname);
        exit(1);
    }
}
