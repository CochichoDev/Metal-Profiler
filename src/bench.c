#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "bench.h"
#include "processHandler.h"

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





/*
uint64_t benchFullConfig(CoreConfig **config, ttyFD tty) {
    char buf[256];
    char script_query[256] = T32SCRIPT;
    uint16_t read_bytes;

    for (size_t idx = 0 ; idx < NUM_CORES ; idx++) {
        if (config)
            strcat(script_query, " \"TRUE\"");
        else
            strcat(script_query, " \"FALSE\"");
    }

    executeTrace32Script(script_query);

    volatile uint8_t STOP=FALSE;
    uint64_t max = 0, curr = 0;
    for (uint32_t idx = 0 ; STOP == FALSE ; idx++) {
        read_bytes = read(tty.fd,buf,255); 
        buf[read_bytes]=0;          
        if (buf[0]=='F') STOP=TRUE;
        if (isdigit(buf[0])) {
            if (idx < IGNORE_LIMIT) continue;       // This is important to ensure we are working in the nominal scenario
            sscanf(buf, "%lu", &curr);
            if (curr > max) max = curr;
        }
    }

    printf("%lu\n", max);
    return max;
}

uint64_t benchIsolationConfig(CoreConfig **config, ttyFD tty) {
    char buf[256];
    char script_query[256] = T32SCRIPT;
    strcat(script_query, " \"TRUE\"");
    strcat(script_query, " \"FALSE\"");
    strcat(script_query, " \"FALSE\"");
    strcat(script_query, " \"FALSE\"");
    uint16_t read_bytes;

    executeTrace32Script(script_query);

    volatile uint8_t STOP=FALSE;
    uint64_t max = 0, curr = 0;
    for (uint32_t idx = 0 ; STOP == FALSE ; idx++) {
        read_bytes = read(tty.fd,buf,255); 
        buf[read_bytes]=0;          
        if (buf[0]=='F') STOP=TRUE;
        if (isdigit(buf[0])) {
            if (idx < IGNORE_LIMIT) continue;       // This is important to ensure we are working in the nominal scenario
            sscanf(buf, "%lu", &curr);

            if (curr > max) max = curr;
        }
    }

    printf("%lu\n", max);
    return max;
}
*/
