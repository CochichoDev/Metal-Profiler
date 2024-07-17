#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "t32.h"
#include "bench.h"
#include "processHandler.h"

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
