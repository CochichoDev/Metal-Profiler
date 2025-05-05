/*
 * File: cli.c
 * CLI FUNCTIONALITY
 * Author: Diogo Cochicho
 */

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include "bench.h"
#include "build.h"
#include "comm.h"
#include "common.h"
#include "cli.h"
#include "default_mod.h"
#include "mmu_gen.h"
#include "state.h"
#include "utils.h"

/*
 * CONSTANT DEFINITION
 */
#define tESC        "\x1B" 
#define tCLEAR      tESC"[2J"
#define tCLEARLINE  tESC"[2K"
#define tHOME       tESC"[H"
#define tHOMELINE   tESC"[0G"
#define tLINEUP     tESC"[1A"

#define WELCOME_MSG     "########################################\n     \
                        \rWELCOME TO AUTOMETALBENCH\n                   \
                        \rWrite \"help\" to see available commands\n      \
                        \r########################################\n"

#define LINE_TRAIL      "\r>>> "

/*
 * TYPE DECLARATION
 */
typedef enum {
    ACTION_NONE,
    ACTION_ANALYZE,
    ACTION_CLEAR,
    ACTION_DEPLOY,
    ACTION_EXECUTE,
    ACTION_EXIT,
    ACTION_GENERATE,
    ACTION_HELP,
    ACTION_LIST,
    ACTION_LOAD,
    ACTION_OPTIMIZE,
    ACTION_SET,
    ACTION_ERROR
} ACTION;

typedef enum {
    G_NONE,
    G_MMU,
    G_LINKER,
    G_ERROR
} GENERATE_ACTION;

typedef enum {
    L_NONE,
    L_CONFIG,
    L_OUTPUT,
    L_ERROR
} LIST_ACTION;

typedef enum {
    O_NONE,
    O_RS,
    O_SA,
    O_ERROR
} OPTIMIZE_ACTION;

typedef enum {
    S_NONE,
    S_ARCH,
    S_OUTPUT,
    S_ERROR
} SET_ACTION;

typedef enum {
    H_NONE,
    H_OUTPUT,
    H_ERROR
} HELP_ACTION;

/************** CLI STATIC FUNCTION DECLARATION ****************/
static ACTION parseAction();
static GENERATE_ACTION parseGenerateArg();
static LIST_ACTION parseListArg();
static OPTIMIZE_ACTION parseOptimizeArg();
static SET_ACTION parseSetArg();
static HELP_ACTION parseHelpArg();
static u8 matchKey(const char *key);
static u8 ignoreLine();
static void getWord(T_PSTR output, size_t max_size);

/************** CLI STATE DECLARATION ****************/
static char lastChar = 0;

/************** CLI WINDOW FUNCTIONS ****************/
/*
 * cliClear: Cleans the output descriptor
 * Return values:
 *      0 : Successful clear
 */
err cliClear() {
    write(STDOUT_FILENO, tCLEAR, strlen(tCLEAR)+1);
    write(STDOUT_FILENO, tHOME, strlen(tHOME)+1);
    return 0;
}

/*
 * cliStart: Starts the tui mode
 * Return values:
 *      0 : Successful initialization
 */
err cliStart() {
    if (cliClear())
        goto lERROR;

    if (dprintf(STDOUT_FILENO, WELCOME_MSG) != strlen(WELCOME_MSG)) goto lERROR;

    while (!cliGetInput());
    return 0;

    lERROR:
        perror("Error: Failed to start cli");
        return 1;
}

/*
 * cliClose: Closes the TUI application
 * Parameters: 
 *      term : Reference to the terminal structure
 * Return values:
 *      0 : Successful exiting
 *      1 : Error exiting terminal
 */
err cliClose() {
    if (cliClear())
        return 1;
    return 0;
}

/************** CLI FUNCTIONALITY ****************/
/*
 * cliGetInput: Gets the action input and dispatches the executuion accordingly
 * Parameters: 
 *      term : Reference to the terminal structure
 * Return values:
 *      0 : Successful execution
 *      1 : Encountered some error with input
 */
err cliGetInput() {
    listState();
    if (dprintf(STDOUT_FILENO, LINE_TRAIL) != strlen(LINE_TRAIL)) return 1;
    char buffer[128] = {0};
    // Additional buffers essential for parsing more actions
    char name[64] = {0};
    char graph[32] = {0};
    char data[32] = {0};
    switch (parseAction()) {
        case ACTION_CLEAR:
            cliClear();
        case ACTION_NONE:
            break;
        case ACTION_ANALYZE:
            analysisTUI();
            break;
        case ACTION_DEPLOY:
            getWord(buffer, 128);
            DEPLOY_FILES(buffer);
            break;
        case ACTION_EXECUTE:
            getWord(buffer, 128);
            runExecution(cliParseNum(buffer), ".");
            break;
        case ACTION_EXIT:
            cliClose();
            cleanState();
            return 1;
        case ACTION_GENERATE:
            switch (parseGenerateArg()) {
                case G_MMU:
                    genMMU(&SELECTED_ARCH);
                    break;
                case G_LINKER:
                    genLinker(&SELECTED_ARCH);
                    break;
                case L_ERROR:
                    return 1;
                case L_NONE:
                    break;
            }
            break;
        case ACTION_HELP:
            switch(parseHelpArg()) {
                case H_OUTPUT:
                    listOutputTypes();
                    break;
                case H_ERROR:
                    return 1;
                case H_NONE:
                    break;
            }
            break;
        case ACTION_LIST:
            switch (parseListArg()) {
                case L_CONFIG:
                    printConfig();
                    break;
                case L_OUTPUT:
                    listSelectedOutputOptions();
                    break;
                case L_ERROR:
                    return 1;
                case L_NONE:
                    listArchs();
                    break;
            }
            break;
        case ACTION_LOAD:
            getWord(buffer, 128);
            loadConfig(cliParseNum(buffer));
            break;
        case ACTION_OPTIMIZE:
            switch (parseOptimizeArg()) {
                case O_RS:
                    getWord(buffer, 128);
                    //optimizeConfig(randomSearchNR, cliParseNum(buffer));
                    break;
                case O_SA:
                    getWord(buffer, 128);
                    //optimizeConfig(simulatedAnnealing, cliParseNum(buffer));
                    break;
                case O_ERROR:
                    return 1;
                case O_NONE:
                    optimizationTUI();
                    break;
            }
            break;
        case ACTION_SET:
            switch (parseSetArg()) {
                case S_ARCH:
                    getWord(buffer, 128);
                    selectArch(cliParseNum(buffer));
                    listConfigs();
                    break;
                case S_OUTPUT:
                    // Get the type of graph
                    getWord(graph, 32);
                    strToUpper(graph);
                    // Get the type of data
                    getWord(data, 32);
                    strToUpper(data);
                    // Get the name
                    getWord(name, 64);
                    //addOutputOption(graph, data, name);
                    break;
                case S_ERROR:
                    return 1;
                case S_NONE:
                    listArchs();
                    break;
            }
            break;
        case ACTION_ERROR:
            return 1;
        default:
            break;
    }

    ignoreLine();
    bzero(buffer, 128);

    return 0;
}

void cliPrintProgress(size_t cur, size_t max) {
    write(STDOUT_FILENO, tLINEUP, strlen(tLINEUP)+1);
    write(STDOUT_FILENO, tCLEARLINE, strlen(tCLEARLINE)+1);
    write(STDOUT_FILENO, tHOMELINE, strlen(tHOMELINE)+1);

    size_t progress = cur * 10 / max;
    write(STDOUT_FILENO, "[", 1);
    for (size_t idx = 0; idx < progress; idx++) 
        write(STDOUT_FILENO, "#", 1);

    for (size_t idx = progress; idx < 10; idx++) 
        write(STDOUT_FILENO, " ", 2);
    write(STDOUT_FILENO, "]\n", 3);
}

/************** CLI PARSING FUNCTIONS ****************/
/*
 * parseAction: Parses the input into actions
 * Parameters: 
 *      in : Input descriptor from where it reads the data
 * Return values:
 *      ERROR : The input is not recognized
 *      EXIT : The programs stops execution
 */
static ACTION parseAction() {
    if (read(STDIN_FILENO, &lastChar, 1) <= 0) goto lERROR;
    if (isalpha(lastChar))
        lastChar &= 0xDF;         // Capitalize letter
    
    switch (lastChar) {
        case 'A':
            // Match for A+NALYZE
            if (matchKey("NALYZE")) return ACTION_ANALYZE;
            goto lNACTION;
        case 'C':
            // Match for C+LEAR
            if (matchKey("LEAR")) return ACTION_CLEAR;
            goto lNACTION;
        case 'D':
            // Match for D+EPLOY
            if (matchKey("EPLOY")) return ACTION_DEPLOY;
            goto lNACTION;
            
        case 'E':
            // Match for E+XIT
            if (matchKey("XIT")) return ACTION_EXIT;

            // Already matched EX
            switch (lastChar) {
                // Match for EXE+CUTE
                case 'E':
                    if (matchKey("CUTE")) return ACTION_EXECUTE;
                default:
                    goto lNACTION;
            }
            goto lNACTION;
        case 'G':
            // Match for G+ENERATE
            if (matchKey("ENERATE")) return ACTION_GENERATE;
            goto lNACTION;
        case 'H':
            // Match for H+ELP
            if (matchKey("ELP")) return ACTION_HELP;
            goto lNACTION;
        case 'L':
            if (matchKey("OAD")) return ACTION_LOAD;
            
            switch (lastChar) {
                case 'I':
                    // Match for LI+ST
                    if (matchKey("ST")) return ACTION_LIST;
                default:
                    goto lNACTION;
            }
            goto lNACTION;
        case 'O':
            // Match for O+PTIMIZE
            if (matchKey("PTIMIZE")) return ACTION_OPTIMIZE;
            goto lNACTION;
        case 'S':
            if (matchKey("ET")) return ACTION_SET;
            goto lNACTION;
        case ' ':
        case '\0':
        case '\t':
            // Recursive call to ignore the blank character
            return parseAction();
            break;
        case '\n':
            return ACTION_NONE;
        default:
            goto lNACTION;
    }

    return 0;

    lERROR:
        perror("Error: Problem reading input\n");
        return ACTION_ERROR;
    lNACTION:
        dprintf(STDOUT_FILENO, "Action not recognized\n");
        return ACTION_NONE;
}

static GENERATE_ACTION parseGenerateArg() {
    while (!isnotblank(lastChar)) {
        if (lastChar == '\n') return G_NONE;
        if (read(STDIN_FILENO, &lastChar, 1) <= 0) goto lERROR;
    }
    if (isalpha(lastChar))
        lastChar &= 0xDF;         // Capitalize letter
    
    switch (lastChar) {
        case 'L':
            // Match for L+INKER
            if (matchKey("INKER")) return G_LINKER;
            goto lNACTION;
        case 'M':
            // Match for M+MU
            if (matchKey("MU")) return G_MMU;
            goto lNACTION;
        case ' ':
        case '\0':
        case '\t':
            // Recursive call to ignore the blank character
            return parseGenerateArg();
            break;
        case '\n':
            return G_NONE;
        default:
            goto lNACTION;
    }

    return 0;

    lERROR:
        perror("Error: Problem reading input\n");
    return G_ERROR;
    lNACTION:
        fprintf(stdout, "Generate action not recognized\n");
        return G_NONE;
}
/*
 * parseListArg: Parses the input into arguments for list action
 * Parameters: 
 *      in : Input descriptor from where it reads the data
 * Return values:
 *      ERROR : The input is not recognized
 *      EXIT : The programs stops execution
 */
static LIST_ACTION parseListArg() {
    while (!isnotblank(lastChar)) {
        if (lastChar == '\n') return L_NONE;
        if (read(STDIN_FILENO, &lastChar, 1) <= 0) goto lERROR;
    }

    if (isalpha(lastChar))
        lastChar &= 0xDF;         // Capitalize letter
    
    switch (lastChar) {
        case 'C':
            // Match for C+ONFIG
            if (matchKey("ONFIG")) return L_CONFIG;
            goto lNACTION;
        case 'O':
            // Match for O+UTPUT
            if (matchKey("UTPUT")) return L_OUTPUT;
            goto lNACTION;
        case ' ':
        case '\0':
        case '\t':
            // Recursive call to ignore the blank character
            return parseListArg();
            break;
        case '\n':
            return L_NONE;
        default:
            goto lNACTION;
    }

    return 0;

    lERROR:
        perror("Error: Problem reading input\n");
    return L_ERROR;
    lNACTION:
        fprintf(stdout, "List action not recognized\n");
        return L_NONE;
}

static OPTIMIZE_ACTION parseOptimizeArg() {
    while (!isnotblank(lastChar)) {
        if (lastChar == '\n') return O_NONE;
        if (read(STDIN_FILENO, &lastChar, 1) <= 0) goto lERROR;
    }

    if (isalpha(lastChar))
        lastChar &= 0xDF;         // Capitalize letter
    
    switch (lastChar) {
        case 'R':
            // Match for R+S
            if (matchKey("S")) return O_RS;
            goto lNACTION;
        case 'S':
            // Match for S+A
            if (matchKey("A")) return O_SA;
            goto lNACTION;
        case ' ':
        case '\0':
        case '\t':
            // Recursive call to ignore the blank character
            return parseOptimizeArg();
            break;
        case '\n':
            return O_NONE;;
        default:
            goto lNACTION;
    }

    return 0;

    lERROR:
        perror("Error: Problem reading input\n");
    return O_ERROR;
    lNACTION:
        fprintf(stdout, "Optimization action not recognized\n");
        return O_NONE;
}

/* parseSetArg: Parses the input into arguments for list action
 * Parameters: 
 *      in : Input descriptor from where it reads the data
 * Return values:
 *      ERROR : The input is not recognized
 *      EXIT : The programs stops execution
 */
static SET_ACTION parseSetArg() {
    while (!isnotblank(lastChar)) {
        if (lastChar == '\n') return S_NONE;
        if (read(STDIN_FILENO, &lastChar, 1) <= 0) goto lERROR;
    }
    if (isalpha(lastChar))
        lastChar &= 0xDF;         // Capitalize letter
    
    switch (lastChar) {
        case 'A':
            // Match for A+RCH
            if (matchKey("RCH")) return S_ARCH;
            goto lNACTION;
        case 'O':
            // Match for O+UTPUT
            if (matchKey("UTPUT")) return S_OUTPUT;
            goto lNACTION;
        case ' ':
        case '\0':
        case '\t':
            // Recursive call to ignore the blank character
            return parseSetArg();
            break;
        case '\n':
            return S_NONE;
        default:
            goto lNACTION;
    }

    return 0;

    lERROR:
        perror("Error: Problem reading input\n");
    return S_ERROR;
    lNACTION:
        fprintf(stdout, "Set action not recognized\n");
        return S_NONE;
}

/* parseSetArg: Parses the input into arguments for list action
 * Parameters: 
 *      in : Input descriptor from where it reads the data
 * Return values:
 *      ERROR : The input is not recognized
 *      EXIT : The programs stops execution
 */
static HELP_ACTION parseHelpArg() {
    while (!isnotblank(lastChar)) {
        if (lastChar == '\n') return H_NONE;
        if (read(STDIN_FILENO, &lastChar, 1) <= 0) goto lERROR;
    }
    if (isalpha(lastChar))
        lastChar &= 0xDF;         // Capitalize letter
    
    switch (lastChar) {
        case 'O':
            // Match for O+UTPUT
            if (matchKey("UTPUT")) return H_OUTPUT;
            goto lNACTION;
        case ' ':
        case '\0':
        case '\t':
            // Recursive call to ignore the blank character
            return parseHelpArg();
            break;
        case '\n':
            return H_NONE;
        default:
            goto lNACTION;
    }

    return 0;

    lERROR:
        perror("Error: Problem reading input\n");
    return H_ERROR;
    lNACTION:
        fprintf(stdout, "Help action not recognized\n");
        return H_NONE;
}


/************** CLI HELPER FUNCTIONS ****************/
/*
 * matchKey: Matches the specified key with the input received from the descriptor
 * Parameters: 
 *      in_descr : Input descriptor from where it reads the data
 *      key : Key to which the input is compared
 * Return values:
 *      1 : The key is matched
 *      0 : The key is different from the input
 */
static u8 matchKey(const char *key) {
    if (read(STDIN_FILENO, &lastChar, 1)) {
        if (*key == '\0') {
            switch (lastChar) {
                case ' ':
                case '\0':
                case '\t':
                case '\n':
                    return 1;
                default:
                    return 0;
            }
        }
        if (lastChar == '\n') return 0;
        lastChar &= 0xDF;
        if (lastChar == *key) return matchKey(key+1);
    }
    return 0;
}

static u8 ignoreLine() {
    while(lastChar != '\n') {
        if (read(STDIN_FILENO, &lastChar, 1) <= 0)
            goto lERROR;
    }
    return 0;

lERROR:
    perror("Error: Could not ignore line\n");
    return 1;
}

static void getWord(T_PSTR output, size_t max_size) {
    while(!isnotblank(lastChar)) {
        if (lastChar == '\n') return;
        read(STDIN_FILENO, &lastChar, 1);
    }
    // Buffer cursor positioned at the first character
    // If the first character is '"' than scan until next '"'
    T_PSTR char_ptr = output;
    if(lastChar == '"') {
        do {
            read(STDIN_FILENO, &lastChar, 1);
            if (lastChar == '\n' || lastChar == '"') break;
            *char_ptr++ = lastChar;
        } while (char_ptr - output < max_size);
    } else {
        while (char_ptr - output < max_size) {
            switch (lastChar) {
                case '\0':
                case '\n':
                case ' ':
                case '\t':
                    goto END;
                default:
                    *char_ptr++ = lastChar;
            }
            read(STDIN_FILENO, &lastChar, 1);
        }
    }

    END:
        *char_ptr = '\0';
}
