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

static ACTION parseAction(TERM *term);
static LIST_ACTION parseListArg(TERM *term);
static SET_ACTION parseSetArg(TERM *term);
static uint8_t matchKey(TERM *term, const char *key);
static uint8_t ignoreLine(TERM *term);
static int64_t parseNumCLI(TERM *term);
static void getWord(TERM *term, STR output, size_t max_size);

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

    if (fcntl(STDIN_FILENO, F_GETFL) == -1 || errno == EBADF) 
        return 2;
    if (fcntl(STDOUT_FILENO, F_GETFL) == -1 || errno == EBADF)
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
    if (write(term->out_descr, LINE_TRAIL, strlen(LINE_TRAIL)+1) != strlen(LINE_TRAIL)+1) return 1;
    char buffer[128];
    switch (parseAction(term)) {
        case CLEAR:
            cliClear(term);
        case NONE:
            break;
        case EXIT:
            cliClose(term);
            return 1;
        case LIST:
            switch (parseListArg(term)) {
                case L_CONFIG:
                    printConfig(term);
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
            loadConfig(buffer);
            break;
        case SET:
            switch (parseSetArg(term)) {
                case S_ARCH:
                    selectArch(term, parseNumCLI(term));
                    break;
                case S_ERROR:
                    return 1;
                case S_NONE:
                    listArchs(term);
                    break;
            }
            break;
        case ERROR:
            //fprintf(stderr, "Error: The specified input does not match to any available action\n");
            return 1;
        default:
            break;
    }

    ignoreLine(term);

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
                default:
                    goto lNACTION;
            }
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
        fprintf(stderr, "Action not recognized\n");
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
        fprintf(stderr, "List action not recognized\n");
        return L_NONE;
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
        fprintf(stderr, "Set action not recognized\n");
        return S_NONE;
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


/*
 * parseNum: Parse a number from the CLI
 * Parameters: 
 *      in_descr : Input descriptor from where it reads the data
 * Return values:
 *      1 : The key is matched
 *      0 : The key is different from the input
 */
static int64_t parseNumCLI(TERM *term) {

    while (!isdigit(term->lastchar)) {
        if (term->lastchar == '\n') return 0;
        read(term->in_descr, &term->lastchar, 1);
    }

    int64_t num = 0;
    while (isdigit(term->lastchar)) {
        num *= 10;
        num += term->lastchar - 0x30;
        read(term->in_descr, &term->lastchar, 1);
    }

    return num;
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

static void getWord(TERM *term, STR output, size_t max_size) {
    while(!isnotblank(term->lastchar)) {
        if (term->lastchar == '\n') return;
        read(term->in_descr, &term->lastchar, 1);
    }
    // Buffer cursor positioned at the first character
    // If the first character is '"' than scan until next '"'
    STR_P char_ptr = output;
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
