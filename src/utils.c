#include "api.h"
#include "global.h"
#include "utils.h"
#include "calc.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

T_PSTR getNameFromPath(T_PSTR path) {
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

void loadAvailableArchs() {
    FILE *archs;

    if (!(archs = fopen(ARCHS_PATH ARCH_CONFIG, "r")))
        perror("Error: Couldn't open architecture configuration file");


    char buf[128];
    int i = 0;
    while (fgets(buf, sizeof(buf), archs)) {
        if (!isalnum(buf[0])) continue;
        if (buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = '\0';          // Get rid of the line feed
       
        memcpy(AVAIL_ARCHS.arch[i].name, buf, strlen(buf)+1);
        memcpy(AVAIL_ARCHS.arch[i].path, ARCHS_PATH, strlen(ARCHS_PATH)+1);
        strcat(AVAIL_ARCHS.arch[i].path, buf);
        strcat(AVAIL_ARCHS.arch[i].path, "/module/bin/dmodule.so");
        i++;
    }
    AVAIL_ARCHS.num = i;

    fclose(archs);
}

void loadAvailableConfigs() {
    FILE *configs;

    
    char config_list_path[256];

    memcpy(config_list_path, ARCHS_PATH, strlen(ARCHS_PATH)+1);
    strcat(config_list_path, SELECTED_ARCH.name);
    strcat(config_list_path, "/configs.list");


    if (!(configs = fopen(config_list_path, "r")))
        perror("Error: Couldn't open configuration list file");

    char buf[128];
    int i = 0;
    while (fgets(buf, sizeof(buf), configs)) {
        if (!isnotblank(buf[0])) continue;
        if (buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = '\0';          // Get rid of the line feed
       
        char *name = getNameFromPath(buf);
        memcpy(AVAIL_CONFIGS.config[i].name, name, strlen(name)+1);
        memcpy(AVAIL_CONFIGS.config[i].path, ARCHS_PATH, strlen(ARCHS_PATH)+1);
        strcat(AVAIL_CONFIGS.config[i].path, SELECTED_ARCH.name);
        strcat(AVAIL_CONFIGS.config[i].path, "/");
        strcat(AVAIL_CONFIGS.config[i].path, buf);
        i++;
    }
    AVAIL_CONFIGS.num = i;

    fclose(configs);
}

int64_t parseNum(T_PSTR str) {
    while (!isdigit(*str)) {
        if (*str == '\n') return 0;
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
    while (!isdigit(*str)) {
        if (*str == '\n') return 0;
        str++;
    }

    int64_t num = 0;
    while (isdigit(str)) {
        num *= 10;
        num += *str - 0x30;
        str++;
    }

    return num;
}

T_VOID strToUpper(T_PSTR str) {
    while (*str != '\0') {
        if (isnotblank(*str)) {
            *str = *str & 0xDF;         // Capitalize letter
        }
        str++;
    }
}

T_UINT numColumnInFile(FILE *file) {
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

T_VOID saveDataMETRICS(const T_PSTR output, G_ARRAY *metrics_array) {
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

T_VOID saveDataRESULTS(const T_PSTR output, G_ARRAY *result_array) {
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
    
    fflush(output_file);
    fclose(output_file);
}

/*
 * saveDataRESULTBATCH : Saves data from an agregate of different RESULT arrays.
 *                          This allows to later plot multiple different scenarios
 *                          with different configurations.
 */
T_ERROR saveDataRESULTBATCH(const T_PSTR output, G_ARRAY *result_array, size_t size_result_array) {
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
