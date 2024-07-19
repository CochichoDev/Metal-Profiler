#pragma once

#include <stdint.h>
#include <unistd.h>

/*
 * CONSTANT DEFINITION
 */
#define tESC     "\x1B"
#define tCLEAR   tESC"[2J"
#define tHOME    tESC"[H"

#define WELCOME_MSG     "########################################\n     \
                        \rWELCOME TO AUTOMETALBENCH\n                   \
                        \rWrite \"help\" to see available commands\n      \
                        \r########################################\n"

#define LINE_TRAIL      "\r>>> "
/*
 * TYPE DECLARATION
 */
typedef struct {
    int in_descr;
    int out_descr;
    char lastchar;
} TERM;

typedef enum {
        NONE,
        EXIT,
        CLEAR,
        LIST,
        LOAD,
        SET,
        ERROR
} ACTION;

typedef enum {
    L_NONE,
    L_CONFIG,
    L_ERROR
} LIST_ACTION;

typedef enum {
    S_NONE,
    S_ARCH,
    S_ERROR
} SET_ACTION;

uint8_t cliInit(TERM *term, int in, int out);
uint8_t cliStart(TERM *term);
uint8_t cliClear(TERM *term);
uint8_t cliGetInput(TERM *term);
uint8_t cliLog(TERM *term);
uint8_t cliClose(TERM *term);

