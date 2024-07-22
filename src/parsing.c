/*
 * Developed by Diogo Cochicho
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "utils.h"
#include "parsing.h"

/*
 * Function definitions
 */

CoreConfig *parseCoreConfig(FILE *fd) {
    CoreConfig *config = (CoreConfig *) malloc(sizeof(CoreConfig));
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), fd)) {
        if (!isalpha(buffer[0])) break;
        if (buffer[0] == '#') {
            uint8_t line_read_size = strlen(buffer);
            fseek(fd, -line_read_size, SEEK_CUR);
            break;
        }
        char *argument = strchr(buffer, ':') + 1;

        char field[32], value[256];
        uint8_t idx = 0;
        // Get the current field
        for (const char *c = buffer; *c != ':' ; c++) {
            if (isalpha(*c)) field[idx++] = *c; 
        }
        field[idx] = '\0';
        idx = 0;
        for (const char *c = argument; *c != '\0' ; c++) {
            if (isalnum(*c)) value[idx++] = *c; 
        }
        value[idx] = '\0';
        
        if (!strcmp(field, "APP")) {
            config->application[0] = '\0';
            strcpy(config->application, value);
        }
        else if (!strcmp(field, "TIMES")) {
            if (isdigit(value[0]))
                config->times = atoi(value);
            else {
                perror("Error: Config file not well structured on TIMES field");
                exit(1);
            }
        }
        else if (!strcmp(field, "SIZE")) {
            if (isdigit(value[0]))
                config->target_size = atoi(value);
            else {
                perror("Error: Config file not well structured on SIZE field");
                exit(1);
            }
        }
        else if (!strcmp(field, "STRIDE")) {
            if (isdigit(value[0]))
                config->stride = atoi(value);
            else {
                perror("Error: Config file not well structured on STRIDE field");
                exit(1);
            }
        }
        else if (!strcmp(field, "LIMIT")) {
            if (isdigit(value[0]))
                config->limit = atoi(value);
            else {
                perror("Error: Config file not well structured on STRIDE field");
                exit(1);
            }
        }
        else {
            fprintf(stderr, "Error: Config file not well structured, field %s was not recognized\n", field);
            exit(1);
        }
    }

    return config;
}

CONFIG *parseConfig(FILE *fd) {
    CONFIG *config = calloc(sizeof(CONFIG), 1);

    char buffer[256];
    size_t line_num = 0;

    COMP *component = NULL;
    pBUFFER *prop_buffer = NULL;
    while (fgets(buffer, sizeof(buffer), fd)) {
        line_num++;
        char *init_buffer_ptr = buffer;
        char *end_buffer_ptr = buffer;

        GET_FIRST_CHAR(init_buffer_ptr);
        if (*init_buffer_ptr == '\n') continue;          // Ignore empty line
        // If the character '#' is detected it initiates a new component with the corresponding ID
        if (*init_buffer_ptr == '#') {
            // Create a new component
            component = calloc(sizeof(COMP), 1);
            component->ID = parseNum(init_buffer_ptr + 1);
            // Create Propriety Buffer and link it to the newly created component
            prop_buffer = calloc(sizeof(pBUFFER), 1);
            component->PBUFFER = prop_buffer;
            // Link an entry of the config components to the newly created component
            config->COMPS[config->NUM] = component;
            config->NUM++;

            printf("New ID: %d\n", component->ID);
            continue;
        }

        GET_FIRST_OCUR(end_buffer_ptr, ':');
        // In case no ':' is found, the propriety is badly formed
        // In case the init ptr and end ptr are the same (no propriety name) throw an error
        if (*end_buffer_ptr == '\n' || \
            init_buffer_ptr == end_buffer_ptr) 
        {
            printf("Error: Parser didn't manage to get propriety value in line %ld\n", line_num);
            return NULL;
        }

        prop_buffer->NUM++;
        prop_buffer->PROPS = realloc(prop_buffer->PROPS, sizeof(PROP)*prop_buffer->NUM);
        PROP *new_prop = prop_buffer->PROPS + prop_buffer->NUM -1;
        memcpy(new_prop->NAME, init_buffer_ptr, end_buffer_ptr-init_buffer_ptr);
        new_prop->NAME[end_buffer_ptr-init_buffer_ptr] = '\0';
        printf("Propriety %s\t", new_prop->NAME);

        end_buffer_ptr++;
        init_buffer_ptr = end_buffer_ptr;       
        GET_FIRST_CHAR(init_buffer_ptr);        // Locate the first character for value
        end_buffer_ptr = init_buffer_ptr;
        GET_FIRST_OCUR(end_buffer_ptr, ' ');    // Make end ptr point towards either end of input or next space

        if (isdigit(*init_buffer_ptr)) {
            if (*(end_buffer_ptr -1) == 'f') {
                new_prop->fINIT = parseFloat(init_buffer_ptr);
                new_prop->PTYPE = pDOUBLE;
            } else {
                new_prop->iINIT = parseNum(init_buffer_ptr);
                new_prop->PTYPE = pINT;

                printf("value %d\n", new_prop->iINIT);
            }
        }
        else if (isalpha(*init_buffer_ptr)) {
            new_prop->PTYPE = pSTR;
            memcpy(new_prop->sINIT, init_buffer_ptr, end_buffer_ptr-init_buffer_ptr);
            new_prop->sINIT[end_buffer_ptr-init_buffer_ptr] = '\0';
            printf("value %s\n", new_prop->sINIT);
        } else {
            printf("Error: Parser didn't manage to get propriety value in line %ld\n", line_num);
            return NULL;
        }
    }

    return config;
}
