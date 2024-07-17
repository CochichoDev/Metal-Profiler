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

uint8_t tuiInit(TERM *term, int in, int out);
uint8_t tuiStart(TERM *term);
uint8_t tuiClear(TERM *term);
uint8_t tuiGetInput(TERM *term);
uint8_t tuiLog(TERM *term);
uint8_t tuiClose(TERM *term);

