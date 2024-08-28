/*
 * File: t32_i.c
 * Trace32 Interface Functions
 * Author: Diogo Cochicho
 */

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "api/t32_i.h"
#include "api.h"
#include "apistate.h"
#include "api/t32.h"

T_ERROR KILL_T32(pid_t T32_pid) {
    if (T32_pid > 0) {
        kill(T32_pid, SIGKILL);
        return 0;
    }
    return -1;
}

/*
 * INIT_T32 : Launches TRACE32 process and connects to it via the t32 api
 * PARAMETERS:
 *      path_to_exec : Path to t32 executable 
 * RETURN:
 *      pid_t : PID of the T32 process or -1 if error
 */
pid_t INIT_T32(T_PSTR path_to_exec) {
    // Get the name of the executable
    T_PSTR slash_marker = path_to_exec;
    T_PSTR last_slash = path_to_exec;
    while (*slash_marker != '\0') {
        if (*slash_marker == '/')
            last_slash = slash_marker;
        slash_marker++;
    }
    last_slash++;

    pid_t t32_pid = RUN_PROCESS_IMAGE(NULL, path_to_exec, last_slash, NULL);
    if (t32_pid == -1)
        return -1;
    sleep(1);
    if (INIT_T32_CONN("localhost", "20000") == -1) {
        fprintf(stderr, "Error: Could not connect to T32, restarting process...\n");
        KILL_T32(t32_pid);
        sleep(1);
        return INIT_T32(path_to_exec);
    }
    return t32_pid;
}


T_INT CLOSE_T32_CONN() {
    if (T32_Exit() != T32_OK) {
        perror("Error: Could not close T32 connection\n");
        return 1;
    }
    return 0;
}

T_INT CLOSE_T32(pid_t t32_pid) {
    CLOSE_T32_CONN();
    if (T32_Exit() != T32_OK) {
        perror("Error: Could not close T32 connection\n");
        return -1;
    }
    sleep(1);
    KILL_PROCESS(t32_pid);
    return 0;
}

T_INT INIT_T32_CONN(const char *node, const char *port) {
    if (T32_Config("NODE=", node) != T32_OK) {
        perror("Error: Could not configure node on T32 connection\n");
        return 1;
    }
    if (T32_Config("PORT=", port) != T32_OK) {
        perror("Error: Could not configure port on T32 connection\n");
        return 1;
    }

    T_UINT max_tries = 3;
    while (max_tries > 0) {
        if (T32_Init() == T32_OK) {
            if (T32_Attach(T32_DEV_ICD) == T32_OK) {
                fprintf(stderr, "Info: Connection to Trace32 established\n");
                break;
            }
        }
        fprintf(stderr, "Error: Could not initilize T32 connection\n");
        fprintf(stderr, "Info: Retrying connection...\n");
        sleep(5);
        max_tries--;
    }
    if (max_tries == 0) {
        return -1;
    }

    fprintf(stderr, "All T32 connection issues were sucessful");
    return 0;
}


T_INT EX_T32_SCRIPT(const char *scriptname) {
    if (T32_Cmd_f("DO %s", scriptname) != T32_OK) {
        fprintf(stderr, "Error: Could not execute %s\n", scriptname);
        return 1;
    }
    return 0;
}
