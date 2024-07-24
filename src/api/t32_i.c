#include <unistd.h>
#include <stdio.h>

#include "api/t32_i.h"
#include "api.h"
#include "api/t32.h"

/*
 * INIT_T32 : Launches TRACE32 process and connects to it via the t32 api
 * PARAMETERS:
 *      path_to_exec : Path to t32 executable 
 * RETURN:
 *      pid_t : PID of the T32 process or -1 if error
 */
pid_t INIT_T32(const T_PSTR path_to_exec) {
    // Get the name of the executable
    T_PSTR slash_marker = path_to_exec;
    while (*path_to_exec != '\0') 
        if (*path_to_exec == '\\') 
            slash_marker = path_to_exec;
    slash_marker++;

    pid_t t32 = RUN_PROCESS_IMAGE(NULL, path_to_exec, slash_marker, NULL);
    if (t32 == -1)
        return -1;
    sleep(1);
    INIT_T32_CONN("localhost", "20000");
    return t32;
}

T_INT CLOSE_T32(pid_t t32_pid) {
    if (T32_Exit() != T32_OK) {
        perror("Error: Could not close T32 connection\n");
        return -1;
    }
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

    while (1) {
        if (T32_Init() == T32_OK) {
            if (T32_Attach(T32_DEV_ICD) == T32_OK) {
                fprintf(stderr, "Info: Connection to Trace32 established\n");
                break;
            }
        }
        fprintf(stderr, "Error: Could not initilize T32 connection\n");
        fprintf(stderr, "Info: Retrying connection...\n");
        sleep(5);
    }

    fprintf(stderr, "All T32 connection issues were sucessful");
    return 0;
}

T_INT CLOSE_T32_CONN() {
    if (T32_Exit() != T32_OK) {
        perror("Error: Could not close T32 connection\n");
        return 1;
    }
    return 0;
}

T_INT EX_T32_SCRIPT(const char *scriptname) {
    if (T32_Cmd_f("DO %s", scriptname) != T32_OK) {
        fprintf(stderr, "Error: Could not execute %s\n", scriptname);
        return 1;
    }
    return 0;
}
