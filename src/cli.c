#include <asm-generic/errno-base.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include <errno.h>

#include "utils.h"
#include "cli.h"
#include "cli_utils.h"
#include "state.h"
#include "optimization.h"
#include "apistate.h"

static ACTION parseAction(TERM *term);
static LIST_ACTION parseListArg(TERM *term);
static OPTIMIZE_ACTION parseOptimizeArg(TERM *term);
static SET_ACTION parseSetArg(TERM *term);
static HELP_ACTION parseHelpArg(TERM *term);
static uint8_t matchKey(TERM *term, const char *key);
static uint8_t ignoreLine(TERM *term);
static void getWord(TERM *term, T_PSTR output, size_t max_size);

/*
 * cliClear: Cleans the output descriptor
 * Parameters: 
 *      term : Reference to the terminal
 * Return values:
 *      0 : Successful clear
 */
uint8_t cliClear(TERM *term) {
    write(term->out_descr, tCLEAR, strlen(tCLEAR)+1);
    write(term->out_descr, tHOME, strlen(tHOME)+1);
    return 0;
}

/*
 * cliInit: Initializes the terminal structure
 * Parameters: 
 *      term : Reference to the terminal structure to be initialized
 *      in : Descriptor of the input
 *      out : Descriptor of the output
 * Return values:
 *      0 : Successful initialization
 *      1 : Invalid term reference
 *      2 : Invalid descriptors
 */
uint8_t cliInit(TERM *term, int in, int out) {
    if (!term)
        return 1;

    if (fcntl(in, F_GETFL) == -1 || errno == EBADF) 
        return 2;
    if (fcntl(out, F_GETFL) == -1 || errno == EBADF)
        return 2;

    term->in_descr = in;
    term->out_descr = out;
    return 0;
}

/*
 * cliStart: Starts the tui mode
 * Parameters: 
 *      term : Reference to the terminal structure to be initialized
 * Return values:
 *      0 : Successful initialization
 */
uint8_t cliStart(TERM *term) {
    if (cliClear(term))
        goto lERROR;

    if (write(term->out_descr, WELCOME_MSG, strlen(WELCOME_MSG)+1) != strlen(WELCOME_MSG)+1) goto lERROR;

    while (!cliGetInput(term));
    return 0;

    lERROR:
        perror("Error: Failed to start cli");
        return 1;
}

/*
 * cliGetInput: Gets the action input and dispatches the executuion accordingly
 * Parameters: 
 *      term : Reference to the terminal structure
 * Return values:
 *      0 : Successful execution
 *      1 : Encountered some error with input
 */
uint8_t cliGetInput(TERM *term) {
    listState();
    if (write(term->out_descr, LINE_TRAIL, strlen(LINE_TRAIL)+1) != strlen(LINE_TRAIL)+1) return 1;
    char buffer[128] = {0};
    // Additional buffers essential for parsing more actions
    char name[64] = {0};
    char graph[32] = {0};
    char data[32] = {0};
    switch (parseAction(term)) {
        case CLEAR:
            cliClear(term);
        case NONE:
            break;
        case EXECUTE:
            getWord(term, buffer, 128);
            runExecution(term, parseNum(buffer));
            break;
        case EXIT:
            cliClose(term);
            cleanState();
            return 1;
        case HELP:
            switch(parseHelpArg(term)) {
                case H_OUTPUT:
                    listOutputTypes();
                    break;
                case H_ERROR:
                    return 1;
                case H_NONE:
                    break;
            }
            break;
        case LIST:
            switch (parseListArg(term)) {
                case L_CONFIG:
                    printConfig(term);
                    break;
                case L_OUTPUT:
                    listSelectedOutputOptions();
                    break;
                case L_ERROR:
                    return 1;
                case L_NONE:
                    listArchs(term);
                    break;
            }
            break;
        case LOAD:
            getWord(term, buffer, 128);
            loadConfig(term, parseNum(buffer));
            break;
        case OPTIMIZE:
            switch (parseOptimizeArg(term)) {
                case O_RS:
                    getWord(term, buffer, 128);
                    //optimizeConfig(randomSearchNR, parseNum(buffer));
                    break;
                case O_SA:
                    getWord(term, buffer, 128);
                    //optimizeConfig(simulatedAnnealing, parseNum(buffer));
                    break;
                case O_ERROR:
                    return 1;
                case O_NONE:
                    optimizationTUI();
                    break;
            }
            break;
        case SET:
            switch (parseSetArg(term)) {
                case S_ARCH:
                    getWord(term, buffer, 128);
                    selectArch(term, parseNum(buffer));
                    listConfigs(term);
                    break;
                case S_OUTPUT:
                    // Get the type of graph
                    getWord(term, graph, 32);
                    strToUpper(graph);
                    // Get the type of data
                    getWord(term, data, 32);
                    strToUpper(data);
                    // Get the name
                    getWord(term, name, 64);
                    addOutputOption(graph, data, name);
                    break;
                case S_ERROR:
                    return 1;
                case S_NONE:
                    listArchs(term);
                    break;
            }
            break;
        case ERROR:
            return 1;
        default:
            break;
    }

    ignoreLine(term);
    bzero(buffer, 128);

    return 0;
}

/*
 * cliClose: Closes the TUI application
 * Parameters: 
 *      term : Reference to the terminal structure
 * Return values:
 *      0 : Successful exiting
 *      1 : Error exiting terminal
 */
uint8_t cliClose(TERM *term) {
    if (cliClear(term))
        return 1;
    return 0;
}

/*
 * parseAction: Parses the input into actions
 * Parameters: 
 *      in : Input descriptor from where it reads the data
 * Return values:
 *      ERROR : The input is not recognized
 *      EXIT : The programs stops execution
 */
static ACTION parseAction(TERM *term) {
    if (read(STDIN_FILENO, &term->lastchar, 1) <= 0) goto lERROR;
    if (isalpha(term->lastchar))
        term->lastchar &= 0xDF;         // Capitalize letter
    
    switch (term->lastchar) {
        case 'C':
            // Match for C+LEAR
            if (matchKey(term, "LEAR")) return CLEAR;
            goto lNACTION;
        case 'E':
            // Match for E+XIT
            if (matchKey(term, "XIT")) return EXIT;

            // Already matched EX
            switch (term->lastchar) {
                // Match for EXE+CUTE
                case 'E':
                    if (matchKey(term, "CUTE")) return EXECUTE;
                default:
                    goto lNACTION;
            }
            goto lNACTION;
        case 'H':
            // Match for H+ELP
            if (matchKey(term, "ELP")) return HELP;
            goto lNACTION;
        case 'L':
            if (matchKey(term, "OAD")) return LOAD;
            
            switch (term->lastchar) {
                case 'I':
                    // Match for LI+ST
                    if (matchKey(term, "ST")) return LIST;
                default:
                    goto lNACTION;
            }
            goto lNACTION;
        case 'O':
            // Match for O+PTIMIZE
            if (matchKey(term, "PTIMIZE")) return OPTIMIZE;
            goto lNACTION;
        case 'S':
            if (matchKey(term, "ET")) return SET;
            goto lNACTION;
        case ' ':
        case '\0':
        case '\t':
            // Recursive call to ignore the blank character
            return parseAction(term);
            break;
        case '\n':
            return NONE;
        default:
            goto lNACTION;
    }

    return 0;

    lERROR:
        perror("Error: Problem reading input\n");
        return ERROR;
    lNACTION:
        fprintf(stdout, "Action not recognized\n");
        return NONE;
}

/*
 * parseListArg: Parses the input into arguments for list action
 * Parameters: 
 *      in : Input descriptor from where it reads the data
 * Return values:
 *      ERROR : The input is not recognized
 *      EXIT : The programs stops execution
 */
static LIST_ACTION parseListArg(TERM *term) {
    while (!isnotblank(term->lastchar)) {
        if (term->lastchar == '\n') return L_NONE;
        if (read(term->in_descr, &term->lastchar, 1) <= 0) goto lERROR;
    }

    if (isalpha(term->lastchar))
        term->lastchar &= 0xDF;         // Capitalize letter
    
    switch (term->lastchar) {
        case 'C':
            // Match for C+ONFIG
            if (matchKey(term, "ONFIG")) return L_CONFIG;
            goto lNACTION;
        case 'O':
            // Match for O+UTPUT
            if (matchKey(term, "UTPUT")) return L_OUTPUT;
            goto lNACTION;
        case ' ':
        case '\0':
        case '\t':
            // Recursive call to ignore the blank character
            return parseListArg(term);
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

static OPTIMIZE_ACTION parseOptimizeArg(TERM *term) {
    while (!isnotblank(term->lastchar)) {
        if (term->lastchar == '\n') return O_NONE;
        if (read(term->in_descr, &term->lastchar, 1) <= 0) goto lERROR;
    }

    if (isalpha(term->lastchar))
        term->lastchar &= 0xDF;         // Capitalize letter
    
    switch (term->lastchar) {
        case 'R':
            // Match for R+S
            if (matchKey(term, "S")) return O_RS;
            goto lNACTION;
        case 'S':
            // Match for S+A
            if (matchKey(term, "A")) return O_SA;
            goto lNACTION;
        case ' ':
        case '\0':
        case '\t':
            // Recursive call to ignore the blank character
            return parseOptimizeArg(term);
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
static SET_ACTION parseSetArg(TERM *term) {
    while (!isnotblank(term->lastchar)) {
        if (term->lastchar == '\n') return S_NONE;
        if (read(term->in_descr, &term->lastchar, 1) <= 0) goto lERROR;
    }
    if (isalpha(term->lastchar))
        term->lastchar &= 0xDF;         // Capitalize letter
    
    switch (term->lastchar) {
        case 'A':
            // Match for A+RCH
            if (matchKey(term, "RCH")) return S_ARCH;
            goto lNACTION;
        case 'O':
            // Match for O+UTPUT
            if (matchKey(term, "UTPUT")) return S_OUTPUT;
            goto lNACTION;
        case ' ':
        case '\0':
        case '\t':
            // Recursive call to ignore the blank character
            return parseSetArg(term);
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
static HELP_ACTION parseHelpArg(TERM *term) {
    while (!isnotblank(term->lastchar)) {
        if (term->lastchar == '\n') return H_NONE;
        if (read(term->in_descr, &term->lastchar, 1) <= 0) goto lERROR;
    }
    if (isalpha(term->lastchar))
        term->lastchar &= 0xDF;         // Capitalize letter
    
    switch (term->lastchar) {
        case 'O':
            // Match for O+UTPUT
            if (matchKey(term, "UTPUT")) return H_OUTPUT;
            goto lNACTION;
        case ' ':
        case '\0':
        case '\t':
            // Recursive call to ignore the blank character
            return parseHelpArg(term);
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

/*
 * matchKey: Matches the specified key with the input received from the descriptor
 * Parameters: 
 *      in_descr : Input descriptor from where it reads the data
 *      key : Key to which the input is compared
 * Return values:
 *      1 : The key is matched
 *      0 : The key is different from the input
 */
static uint8_t matchKey(TERM *term, const char *key) {
    if (read(term->in_descr, &term->lastchar, 1)) {
        if (*key == '\0') {
            switch (term->lastchar) {
                case ' ':
                case '\0':
                case '\t':
                case '\n':
                    return 1;
                default:
                    return 0;
            }
        }
        if (term->lastchar == '\n') return 0;
        term->lastchar &= 0xDF;
        if (term->lastchar== *key) return matchKey(term, key+1);
    }
    return 0;
}

static uint8_t ignoreLine(TERM *term) {
    while(term->lastchar != '\n') {
        if (read(term->in_descr, &term->lastchar, 1) <= 0)
            goto lERROR;
    }
    return 0;

lERROR:
    perror("Error: Could not ignore line\n");
    return 1;
}

static void getWord(TERM *term, T_PSTR output, size_t max_size) {
    while(!isnotblank(term->lastchar)) {
        if (term->lastchar == '\n') return;
        read(term->in_descr, &term->lastchar, 1);
    }
    // Buffer cursor positioned at the first character
    // If the first character is '"' than scan until next '"'
    T_PSTR char_ptr = output;
    if(term->lastchar == '"') {
        do {
            read(term->in_descr, &term->lastchar, 1);
            if (term->lastchar == '\n' || term->lastchar == '"') break;
            *char_ptr++ = term->lastchar;
        } while (char_ptr - output < max_size);
    } else {
        while (char_ptr - output < max_size) {
            switch (term->lastchar) {
                case '\0':
                case '\n':
                case ' ':
                case '\t':
                    goto END;
                default:
                    *char_ptr++ = term->lastchar;
            }
            read(term->in_descr, &term->lastchar, 1);
        }
    }

    END:
        *char_ptr = '\0';
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
