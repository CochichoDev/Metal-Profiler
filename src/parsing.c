/*
 * Developed by Diogo Cochicho
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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
