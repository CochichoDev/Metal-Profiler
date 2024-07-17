/*
 * Developed by Diogo Cochicho
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "parsing.h"

void getInitialConfig(const char *input, CoreConfig **config) {
    FILE *input_fd = fopen(input, "r");
    if (input == NULL) {
        perror("Error: Could not open input file");
        exit(1);
    }

    char buffer[256];
    while (!feof(input_fd) && fgets(buffer, sizeof(buffer), input_fd)) {
        if (buffer[0] == '#') {
            for (const char *c = buffer ; *c != '\0' ; c++)
                if (isdigit(*c)) {
                    /*
                     * Create new config instance
                     */
                    if (*c - '0' > 6) {
                        perror("Error: Described core out of range");
                        exit(1);
                    }
                    config[*c - '0'] = parseCoreConfig(input_fd);
                    /*
                    printf("New core config created\n MODE: %d\n APP: %s\n TIMES: %d\n SIZE: %d\n STRIDE: %d\n", \
                           config[*c - '0']->mode, config[*c - '0']->application, config[*c - '0']->times, config[*c - '0']->target_size, config[*c - '0']->stride);
                    */
                    break;
                }
        }
    }
    fclose(input_fd);
}

const char *makeString(CoreConfig *config) {
    char *CFLAGS = (char *) malloc(512);
    memcpy(CFLAGS, "CFLAGS=", 8);
    char mode_def[16] = "-D";
    char app_def[16] = "-D";
    //char times_def[16] = "-D_";
    char size_def[32] = "-DTARGET_SIZE=";
    char stride_def[32] = "-DSTRIDE=";
    char limit_def[32] = "-DLIMIT=";
    strcat(mode_def, (config->mode) ? "VICTIM" : "ENEMY");
    strcat(app_def, config->application);
    //char times_str[4];
    //sprintf(times_str, "%d", config->times);
    //strcat(times_def, times_str);
    char size_str[16];
    sprintf(size_str, "%d", config->target_size);
    strcat(size_def, size_str);
    strcat(size_def, " ");
    char stride_str[32];
    sprintf(stride_str, "%d", config->stride);
    strcat(stride_def, stride_str);
    char limit_str[16];
    sprintf(limit_str, "%d", config->limit);
    strcat(limit_def, limit_str);

    
    //strcat(CFLAGS, mode_def);
    //strcat(CFLAGS, " ");
    strcat(CFLAGS, app_def);
    strcat(CFLAGS, " ");
    //strcat(CFLAGS, times_def);
    //strcat(CFLAGS, " ");
    strcat(CFLAGS, size_def);
    strcat(CFLAGS, " ");
    strcat(CFLAGS, stride_def);
    strcat(CFLAGS, " ");
    strcat(CFLAGS, limit_def);

    return CFLAGS;
}

/*
 *  Simply initializes the struct to all zeros and
 *  the pointer to a memory block big enough to hold
 *  the victim results
 */
void uint64_t_initializeResults(void *results_ptr, uint64_t num_cycles, const char *name) {
    RESULTS(uint64_t) *results = results_ptr;
    if (!results) {
        perror("Error: Null results pointer cannot be derefenced\n");
        exit(1);
    }
    strcpy(results->name, name);
    results->median = 0;
    results->max = 0;
    results->min = 0;
    results->average = 0;
    results->std_deviation = 0;
    results->cycles = (uint64_t *) malloc(sizeof(uint64_t)*num_cycles);
    results->num_cycles = num_cycles;
}
void double_initializeResults(void *results_ptr, uint64_t num_cycles, const char *name) {
    RESULTS(double) *results = results_ptr;
    if (!results) {
        perror("Error: Null results pointer cannot be derefenced\n");
        exit(1);
    }
    strcpy(results->name, name);
    results->median = 0;
    results->max = 0;
    results->min = 0;
    results->average = 0;
    results->std_deviation = 0;
    results->cycles = (double *) malloc(sizeof(double)*num_cycles);
    results->num_cycles = num_cycles;
}

void uint64_t_destroyResults(void *results_ptr) {
    RESULTS(uint64_t) *results = results_ptr;
    if (!results) {
        perror("Error: Null results pointer cannot be derefenced\n");
        exit(1);
    }
    free(results->cycles);
}
void double_destroyResults(void *results_ptr) {
    RESULTS(double) *results = results_ptr;
    if (!results) {
        perror("Error: Null results pointer cannot be derefenced\n");
        exit(1);
    }
    free(results->cycles);
}
