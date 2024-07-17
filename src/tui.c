#include <asm-generic/errno-base.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <errno.h>

#include "tui.h"

static ACTION parseAction(TERM *term);
static uint8_t matchKey(TERM *term, const char *key);
static uint8_t ignoreLine(TERM *term);

/*
 * tuiClear: Cleans the output descriptor
 * Parameters: 
 *      term : Reference to the terminal
 * Return values:
 *      0 : Successful clear
 */
uint8_t tuiClear(TERM *term) {
    write(term->out_descr, tCLEAR, strlen(tCLEAR)+1);
    write(term->out_descr, tHOME, strlen(tHOME)+1);
    return 0;
}

/*
 * tuiInit: Initializes the terminal structure
 * Parameters: 
 *      term : Reference to the terminal structure to be initialized
 *      in : Descriptor of the input
 *      out : Descriptor of the output
 * Return values:
 *      0 : Successful initialization
 *      1 : Invalid term reference
 *      2 : Invalid descriptors
 */
uint8_t tuiInit(TERM *term, int in, int out) {
    if (!term)
        return 1;

    if (fcntl(STDIN_FILENO, F_GETFL) == -1 || errno == EBADF) 
        return 2;
    if (fcntl(STDOUT_FILENO, F_GETFL) == -1 || errno == EBADF)
        return 2;

    term->in_descr = in;
    term->out_descr = out;
    return 0;
}

/*
 * tuiStart: Starts the tui mode
 * Parameters: 
 *      term : Reference to the terminal structure to be initialized
 * Return values:
 *      0 : Successful initialization
 */
uint8_t tuiStart(TERM *term) {
    if (tuiClear(term))
        goto lERROR;

    if (write(term->out_descr, WELCOME_MSG, strlen(WELCOME_MSG)+1) != strlen(WELCOME_MSG)+1) goto lERROR;

    while (!tuiGetInput(term));
    return 0;

    lERROR:
        perror("Error: Failed to start TUI");
        return 1;
}

/*
 * tuiGetInput: Gets the action input and dispatches the executuion accordingly
 * Parameters: 
 *      term : Reference to the terminal structure
 * Return values:
 *      0 : Successful execution
 *      1 : Encountered some error with input
 */
uint8_t tuiGetInput(TERM *term) {
    if (write(term->out_descr, LINE_TRAIL, strlen(LINE_TRAIL)+1) != strlen(LINE_TRAIL)+1) return 1;
    ACTION input = parseAction(term);
    switch (input) {
        case CLEAR:
            tuiClear(term);
        case NONE:
            ignoreLine(term);
            break;
        case EXIT:
            tuiClose(term);
            return 1;
        case LIST:
            puts("Execute List function");
            break;
        case LOAD:
            puts("Execute Load function");
            break;
        case SET:
            puts("SET function");
            break;
        case ERROR:
            //fprintf(stderr, "Error: The specified input does not match to any available action\n");
            return 1;
        default:
            break;
    }

    return 0;
}

/*
 * tuiClose: Closes the TUI application
 * Parameters: 
 *      term : Reference to the terminal structure
 * Return values:
 *      0 : Successful exiting
 *      1 : Error exiting terminal
 */
uint8_t tuiClose(TERM *term) {
    if (tuiClear(term))
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
                    if (matchKey(term, "CUTE")) return EXIT;
                default:
                    goto lNACTION;
            }
            goto lNACTION;
        case 'L':
            if (matchKey(term, "OAD")) return LOAD;
            
            switch (term->lastchar) {
                case 'I':
                    // Match for LI+ST
                    if (matchKey(term, "ST")) return LIST;
                    goto lNACTION;
                default:
                    goto lNACTION;
            }
            goto lNACTION;
        case 'S':
            if (matchKey(term, "ET")) return SET;
            goto lNACTION;
        case ' ':
        case '\0':
        case '\n':
        case '\t':
            return NONE;
        default:
            goto lNACTION;
    }

    return 0;

    lERROR:
        perror("Error: Problem reading input\n");
        return ERROR;
    lNACTION:
        fprintf(stderr, "Action not recognized\n");
        return NONE;
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
    if (*key == '\0') return 1;
    if (read(term->in_descr, &term->lastchar, 1)) {
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
}
