/*
 * File: utils.c
 * General functionality used/usefull for different scenarios
 * Author: Diogo Cochicho
 */

#include "utils.h"
#include "api.h"
#include "calc.h"
#include "global.h"
#include "types.h"
#include "optimization.h"
#include "common.h"

#include <alloca.h>
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wchar.h>



/************** PARSING FUNCTIONS ****************/
size_t itos(int num, char *str) {
    if (num == 0) {
        *str++ = '0';
        *str = 0;
        return 1;
    }
    char buf[16];
    size_t idx = 0;
    while (num) {
        buf[idx++] = num % 10 + 0x30;
        num /= 10;
    }

    size_t size = idx;
        
    while (idx > 0) {
        *str++ = buf[--idx];
    }

    *str = '\0';
    return size;
}

s64 cliParseNum(const char *str) {
    if (str == NULL || *str == '\0') return 0;
    while (!isdigit(*str)) {
        if (*str == '\n' || *str == '\0') return 0;
        str++;
    }

    int64_t num = 0;
    while (isdigit(*str)) {
        num *= 10;
        num += *str - 0x30;
        str++;
    }

    return num;
}

double parseFloat(char *str) {
    T_FLAG decimal_part = FALSE;
    while (!isdigit(*str)) {
        if (*str == '\n') return 0;
        str++;
    }

    double num = 0;
    double multiplier = 1;
    while (isgraph(*str)) {
        if (*str == '.') {
            decimal_part = TRUE;
        } else if (isdigit(*str)) {
            if (decimal_part) {
                multiplier *= 0.1f;
                num += ((*str - '0') * multiplier);
            } else {
                num *= 10;
                num += *str - '0';
            }
        } else {
            fprintf(stderr, "Error: Could not parse float, unrecognized character %c\n", *str);
            return num;
        }
        str++;
    }

    return num;
}

/************** STRING HANDLING FUNCTIONS ****************/
void strToUpper(char *str) {
    while (*str != '\0') {
        if (isnotblank(*str)) {
            *str = *str & 0xDF;         // Capitalize letter
        }
        str++;
    }
}

char *getNameFromPath(char *path) {
    // Get the name of the executable
    T_PSTR slash_marker = path;
    T_PSTR after_slash = path;
    while (*slash_marker != '\0') {
        if (*slash_marker == '/')
            after_slash = slash_marker+1;
        slash_marker++;
    }

    return after_slash;
}

char *replaceChar(char *string, char init, char replace) {
    assert(string != NULL);
    T_PSTR ptr = string;
    while (*ptr != '\0') {
        if (*ptr == init) *ptr = replace;
        ptr++;
    }
    return string;
}

/************** FILE HANDLING FUNCTIONS ****************/
u32 numColumnInFile(FILE *file) {
    if (!file)
        return 0;

    // Save offset
    T_INT offset_file = ftell(file);
    // Put cursor in beggining of the file
    fseek(file, 0, SEEK_SET);

    T_UINT num_columns = 0;
    T_CHAR c_buf = fgetc(file);
    while (c_buf != '\n') {
        while (!isnotblank(c_buf)) {
            if (c_buf == '\n')
                goto END;
            c_buf = fgetc(file);
        }
        num_columns++;
        while (isnotblank(c_buf)) {
            c_buf = fgetc(file);
        }
    }

END:
    fseek(file, SEEK_SET, offset_file);
    return num_columns;
}

void saveDataMETRICS(char *const output, G_ARRAY *metrics_array) {
    if (!metrics_array || !metrics_array->DATA) {
        fprintf(stderr, "Error: saveDataMETRICS called with NULL metrics_array data\n");
        return;
    }
    FILE *output_file = fopen(output, "w");
    if (!output_file) {
        fprintf(stderr, "Error: Could not open %s to save metrics", output);
        return;
    }


    fprintf(output_file, "%12s", "NAME");
    fprintf(output_file, "%12s", "MEDIAN");
    fprintf(output_file, "%12s", "MIN");
    fprintf(output_file, "%12s", "MAX");
    fprintf(output_file, "\n");

    METRICS *data = metrics_array->DATA;

    for (size_t r_idx = 0 ; r_idx < metrics_array->SIZE ; r_idx++) {
        fprintf(output_file, "%12s", data[r_idx].NAME);
        switch (data[r_idx].MEDIAN.TYPE) {
            case G_INT:
            case G_UINT:
                fprintf(output_file, "%12d", *((T_UINT*) data[r_idx].MEDIAN.DATA));
                break;
            case G_DOUBLE:
                fprintf(output_file, "%12lf", *((T_DOUBLE*) data[r_idx].MEDIAN.DATA));
            default:
                break;
        }
        switch (data[r_idx].MIN.TYPE) {
            case G_INT:
            case G_UINT:
                fprintf(output_file, "%12d", *((T_UINT*) data[r_idx].MIN.DATA));
                break;
            case G_DOUBLE:
                fprintf(output_file, "%12lf", *((T_DOUBLE*) data[r_idx].MIN.DATA));
            default:
                break;

        }
        switch (data[r_idx].MAX.TYPE) {
            case G_INT:
            case G_UINT:
                fprintf(output_file, "%12d", *((T_UINT*) data[r_idx].MAX.DATA));
                break;
            case G_DOUBLE:
                fprintf(output_file, "%12lf", *((T_DOUBLE*) data[r_idx].MAX.DATA));
            default:
                break;

        }
        fprintf(output_file, "\n");
    }
    
    fflush(output_file);
    fclose(output_file);
}

void saveDataRESULTS(char *const output, G_ARRAY *result_array) {
    RESULT *result_data = result_array->DATA;
    FILE *output_file = fopen(output, "w");
    if (!output_file) {
        fprintf(stderr, "Error: Could not open %s to save results", output);
        return;
    }

    size_t max_rows = 0;
    fprintf(output_file, "%12s", "IDX");
    for (size_t idx = 0 ; idx < result_array->SIZE ; idx++) {
        if (result_data[idx].ARRAY.SIZE > max_rows) max_rows = result_data[idx].ARRAY.SIZE;
        fprintf(output_file, "%12s", result_data[idx].NAME);
    }
    fprintf(output_file, "\n");

    /*
     * IMPORTANT NOTICE
     * It's important that each value in DATA of each RESULT is initialized
     * If RESULT->NUM is not equal to the real size of the initialized DATA
     * some problems may arise. Valgrind throws some warnings about it.
     */
    for (size_t r_idx = 0 ; r_idx < max_rows ; r_idx++) {
        fprintf(output_file, "%12ld", r_idx);
        for (size_t c_idx = 0 ; c_idx < result_array->SIZE ; c_idx++) {
            if (r_idx >= result_data[c_idx].ARRAY.SIZE) {
                fprintf(output_file, "%12s", " ");
            } else {
                switch (result_data[c_idx].ARRAY.TYPE) {
                    case G_INT:
                    case G_UINT:
                        fprintf(output_file, "%12d", ((T_UINT*) result_data[c_idx].ARRAY.DATA)[r_idx]);
                        break;
                    case G_DOUBLE:
                        fprintf(output_file, "%12lf", ((T_DOUBLE*) result_data[c_idx].ARRAY.DATA)[r_idx]);
                        break;
                    default:
                        break;
                }
            }
        }
        fprintf(output_file, "\n");
    }
    
    fflush(output_file);
    fclose(output_file);
}

/*
 * saveDataRESULTBATCH : Saves data from an agregate of different RESULT arrays.
 *                          This allows to later plot multiple different scenarios
 *                          with different configurations.
 */
err saveDataRESULTBATCH(char *const output, G_ARRAY *result_array, size_t size_result_array) {
    FILE *output_file = fopen(output, "w");
    if (!output_file)
        return -1;
    if (!result_array)
        return -1;
    if (!strlen(output))
        return -1;
    if (size_result_array <= 0)
        return -1;

    size_t max_rows = 0;
    fprintf(output_file, "%12s", "IDX");
    for (size_t array_idx = 0 ; array_idx < size_result_array ; array_idx++) {
        G_ARRAY *array_elem = result_array + array_idx;

        for (size_t result_idx = 0 ; result_idx < array_elem->SIZE; result_idx++) {
            RESULT *result_data = array_elem->DATA;

            if (result_data[result_idx].ARRAY.SIZE > max_rows) max_rows = result_data[result_idx].ARRAY.SIZE;
            fprintf(output_file, "%12s", result_data[result_idx].NAME);
        }
    }
    fprintf(output_file, "\n");

    for (size_t array_idx = 0; array_idx < size_result_array; array_idx++) {
        G_ARRAY *array_elem = result_array + array_idx;
        RESULT *result_data = array_elem->DATA;

        for (size_t r_idx = 0 ; r_idx < max_rows ; r_idx++) {
            fprintf(output_file, "%12ld", r_idx);
            for (size_t c_idx = 0 ; c_idx < array_elem->SIZE ; c_idx++) {
                if (r_idx >= result_data[c_idx].ARRAY.SIZE) {
                    fprintf(output_file, "%12d", 0);
                } else {
                    switch (result_data[c_idx].ARRAY.TYPE) {
                        case G_INT:
                        case G_UINT:
                            fprintf(output_file, "%12d", ((T_UINT*) result_data[c_idx].ARRAY.DATA)[r_idx]);
                            break;
                        case G_DOUBLE:
                            fprintf(output_file, "%12lf", ((T_DOUBLE*) result_data[c_idx].ARRAY.DATA)[r_idx]);
                        default:
                            break;
                    }
                }
            }
            fprintf(output_file, "\n");
        }
    }
    
    fclose(output_file);
    return 0;
}

err saveDataOptimizationResults(char *const output, G_ARRAY *optimization_array, OPT_MAP *map) {
    assert(optimization_array->TYPE == G_OPTRESULT);
    FILE *output_file = fopen(output, "w");
    if (!output_file)
        return -1;
    if (!optimization_array)
        return -1;
    if (!strlen(output))
        return -1;

    fprintf(output_file, "%12s", "IDX");
    fprintf(output_file, "%12s", "DEG");
    size_t param_columns = 0;
    for (size_t row_idx = 0; row_idx < map->NUM_COMP; row_idx++) {
        for (size_t param_idx = 0; param_idx < map->PROPS_P_ROW[row_idx]; param_idx++) {
            fprintf(output_file, "%11s%ld", "PARAM", param_columns);
            param_columns++;
        }
    }
    fprintf(output_file, "\n");


    for (size_t r_idx = 0 ; r_idx < optimization_array->SIZE ; r_idx++) {
        fprintf(output_file, "%12ld", r_idx);
        fprintf(output_file, "%12lf", ((OPT_RESULT *)optimization_array->DATA)[r_idx].DEG);
        printParameterGridFILE(output_file, map, ((OPT_RESULT *)optimization_array->DATA)[r_idx].GRID);
        fprintf(output_file, "\n");
    }
    
    fclose(output_file);
    return 0;
}

/* 
 * cp_dir2dir: Copy all files from specified directory to destination path 
 */
err cp_dir2dir(const char *src, const char *dest) {
    DIR *srcDir;
    s32 dirFD, destFD;

    if (!(srcDir = opendir(src))) {
        perror("Could not open source directory");
        return -1;
    }
    dirFD = dirfd(srcDir);

    if ((destFD = open(dest, O_DIRECTORY)) == -1) {
        perror("Could not open destination directory");
        return -1;
    }

    s32 srcFile, destFile;

    struct dirent *entry;
    while ((entry = readdir(srcDir))) {
        if (entry->d_type == DT_DIR) continue;      /* Ignore directories */

        if ((srcFile = openat(dirFD, entry->d_name, O_RDONLY)) == -1) {
            dprintf(STDOUT_FILENO, "Could not open source file: %s\tError:  %d\n", entry->d_name, errno);
            return -1;
        }
        if ((destFile = openat(destFD, entry->d_name, O_CREAT | O_RDWR)) == -1) {
            dprintf(STDOUT_FILENO, "Could not create destination file: %s\tError:  %d\n", entry->d_name, errno);
            return -1;
        }

        /* Since sendfiles requires a size it will transfer an unknown amount of 1kB blocks */
        while(1) {
            size_t bytes;
            if ((bytes = sendfile(destFile, srcFile, NULL, 1024)) < 1024) {
                if (bytes == -1) {
                    dprintf(STDOUT_FILENO, "Failed to copy %s\tError:  %d\n", entry->d_name, errno);
                    return -1;
                }
                break;
            }
        }
    }

    return 0;
}

/************** CONFIG HANDLING FUNCTIONS ****************/
size_t strProprietyIdxByPtr(char **OPTS, char *prop) {
    size_t idx = 0;
    for ( ; OPTS[idx] != NULL; idx++) {
        if (prop == OPTS[idx]) break;
    }
    return idx;
}

size_t strProprietyIdxByValue(char **OPTS, char *prop) {
    size_t idx = 0;
    while (OPTS[idx] != NULL) {
        if (!strcmp(prop, OPTS[idx])) break;
        idx++;
    }
    return idx;
}

CONFIG *const cloneConfig(CONFIG *const cfg) {
    assert(cfg != NULL);

    CONFIG *const clone = calloc(1, sizeof(CONFIG));

    clone->NUM = cfg->NUM;
    clone->VICTIM_ID = cfg->VICTIM_ID;
    for (size_t idx = 0; idx < cfg->NUM; idx++) {
        clone->COMPS[idx] = calloc(1, sizeof(COMP));
        clone->COMPS[idx]->ID = cfg->COMPS[idx]->ID;
        strncpy(clone->COMPS[idx]->NAME, cfg->COMPS[idx]->NAME, sizeof(T_STR));
        clone->COMPS[idx]->PBUFFER = calloc(1, sizeof(pBUFFER));
        clone->COMPS[idx]->PBUFFER->NUM = cfg->COMPS[idx]->PBUFFER->NUM;
        clone->COMPS[idx]->PBUFFER->PROPS = calloc(cfg->COMPS[idx]->PBUFFER->NUM, sizeof(PROP));

        for (size_t prop_idx = 0; prop_idx < cfg->COMPS[idx]->PBUFFER->NUM; prop_idx++) {
            PROP *cur_prop = clone->COMPS[idx]->PBUFFER->PROPS + prop_idx;
            memcpy(cur_prop, cfg->COMPS[idx]->PBUFFER->PROPS + prop_idx, sizeof(PROP));
        }
    }

    return clone;
}

/*
 * This functions results in true if any optimizable
 * propriety is not a marked as mitigation
 */
s8 isConfigAlwaysOptimizable(CONFIG *cfg) {
    COMP *comp = NULL;
    for (size_t comp_idx = 0; comp_idx < cfg->NUM; comp_idx++) {
        comp = cfg->COMPS[comp_idx];
        for (size_t prop_idx = 0; prop_idx < comp->PBUFFER->NUM; prop_idx++) {
            PROP *prop = &comp->PBUFFER->PROPS[prop_idx];
            if (IS_OPTIMIZABLE(prop->FLAGS) && !IS_MITIGATION(prop->FLAGS)) return TRUE;
        }
    }
    return FALSE;
}

/************** MAKEFILE HANDLING FUNCTIONS ****************/

