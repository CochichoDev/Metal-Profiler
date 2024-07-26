#include "plot.h"
#include "api.h"
#include "global.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void __T_INT_saveDataRESULTS(const char *output, RESULT *data, size_t size) {
    char result_file_name[128];
    strcpy(result_file_name, output);
    strcat(result_file_name, "_results");
    
    FILE *output_file = fopen(result_file_name, "w");

    size_t max_rows = 0;
    for (size_t idx = 0 ; idx < size ; idx++) {
        if (data[idx].NUM > max_rows) max_rows = data[idx].NUM;
    }

    /*
     * IMPORTANT NOTICE
     * It's important that each value in DATA of each RESULT is initialized
     * If RESULT->NUM is not equal to the real size of the initialized DATA
     * some problems may arise. Valgrind throws some warnings about it.
     */
    for (size_t r_idx = 0 ; r_idx < max_rows ; r_idx++) {
        for (size_t c_idx = 0 ; c_idx < size ; c_idx++) {
            if (r_idx >= data[c_idx].NUM) {
                fprintf(output_file, "%9d", 0);
            } else {
                fprintf(output_file, "%9d", ((T_UINT*) data[c_idx].DATA)[r_idx]);
            }
        }
        fprintf(output_file, "\n");
    }
    
    fclose(output_file);
}

void __T_DOUBLE_saveDataRESULTS(const char *output, RESULT *data, size_t size) {
    char result_file_name[128];
    strcpy(result_file_name, output);
    strcat(result_file_name, "_results");
    
    FILE *output_file = fopen(result_file_name, "w");

    size_t max_rows = 0;
    for (size_t idx = 0 ; idx < size ; idx++) {
        if (data[idx].NUM > max_rows) max_rows = data[idx].NUM;
    }

    for (size_t r_idx = 0 ; r_idx < max_rows ; r_idx++) {
        for (size_t c_idx = 0 ; c_idx < size ; c_idx++) {
            if (data[c_idx].NUM < r_idx) {
                fprintf(output_file, "%9lf", .0);
            } else {
                fprintf(output_file, "%9lf", ((T_DOUBLE*) data[c_idx].DATA)[r_idx]);
            }
        }
        fprintf(output_file, "\n");
    }
    
    fclose(output_file);
}

void __T_INT_saveDataVAR(const char *output, ...) {
    char result_file_name[128];
    strcpy(result_file_name, output);
    strcat(result_file_name, "_results");
    
    FILE *output_file = fopen(result_file_name, "w");

    va_list va;
    va_start(va, output);
    
    T_VOID *ptr_buf;
    size_t num_buf;
    size_t num_columns = 0;
    T_INT **column_ptr = NULL;
    size_t *num_points = NULL;

    size_t max_lines = 0;
    while ((ptr_buf = va_arg(va, T_VOID*))) {
        num_columns++;
        column_ptr = realloc(column_ptr, sizeof(T_INT*) * num_columns);
        column_ptr[num_columns-1] = ptr_buf;
        num_buf = va_arg(va, size_t);
        num_points = realloc(column_ptr, sizeof(size_t) * num_columns);
        num_points[num_columns-1] = num_buf;
         if (max_lines < num_buf) max_lines = num_buf;
    }


    for (size_t r_idx = 0 ; r_idx < max_lines ; r_idx++) {
        for (size_t c_idx = 0 ; c_idx < num_columns ; c_idx++) {
            if (r_idx > num_points[c_idx]) {
                fprintf(output_file, "%9d", 0);
            }
            fprintf(output_file, "%9d", column_ptr[c_idx][r_idx]);
        }
        fprintf(output_file, "\n");
    }
    
    va_end(va);
    fclose(output_file);
    if (column_ptr != NULL)
        free(column_ptr);
    if (num_points != NULL)
        free(num_points);
}
void __T_DOUBLE_saveDataVAR(const char *output, ...) {
    char result_file_name[128];
    strcpy(result_file_name, output);
    strcat(result_file_name, "_results");
    
    FILE *output_file = fopen(result_file_name, "w");

    va_list va;
    va_start(va, output);
    
    T_VOID *ptr_buf;
    size_t num_buf;
    size_t num_columns = 0;
    T_DOUBLE **column_ptr = NULL;
    size_t *num_points = NULL;

    size_t max_lines = 0;
    while ((ptr_buf = va_arg(va, T_VOID*))) {
        num_columns++;
        column_ptr = realloc(column_ptr, sizeof(T_DOUBLE *) * num_columns);
        column_ptr[num_columns-1] = ptr_buf;
        num_buf = va_arg(va, size_t);
        num_points = realloc(column_ptr, sizeof(size_t) * num_columns);
        num_points[num_columns-1] = num_buf;
         if (max_lines < num_buf) max_lines = num_buf;
    }


    for (size_t r_idx = 0 ; r_idx < max_lines ; r_idx++) {
        for (size_t c_idx = 0 ; c_idx < num_columns ; c_idx++) {
            if (r_idx > num_points[c_idx]) {
                fprintf(output_file, "%9lf", .0);
            }
            fprintf(output_file, "%9lf", column_ptr[c_idx][r_idx]);
        }
        fprintf(output_file, "\n");
    }
    
    va_end(va);
    fclose(output_file);
    if (column_ptr != NULL)
        free(column_ptr);
    if (num_points != NULL)
        free(num_points);
}

void plotScatter(const char *output) {
    char result_file_name[128];
    strcpy(result_file_name, output);
    strcat(result_file_name, "_results");

    FILE *gnuplot = popen("gnuplot", "w");

    const char *output_cmd = "set output \'%s.png\'";
    const char *plot_cmd = "plot \'%s\' using 2:3:4:xticlabels(1) title columnheader";
    char gnuplot_cmds[][128] = {
                        "clear",
                        "reset",
                        "unset key",
                        "set terminal png",
                        "set xtics rotate out",
                        "set style data histogram",
                        "set style histogram errorbars linewidth 1",
                        "set style fill solid 0.3",
                        "set bars front",
                        "",
                        "",
                        "exit"
                        };

    sprintf(gnuplot_cmds[sizeof(gnuplot_cmds)/128 -3], output_cmd, output);
    sprintf(gnuplot_cmds[sizeof(gnuplot_cmds)/128 -2], plot_cmd, result_file_name);

    for (size_t idx = 0 ; idx < (sizeof(gnuplot_cmds)/128) ; idx++) {
        fprintf(gnuplot,"%s\n", gnuplot_cmds[idx]);
        fflush(gnuplot);
    }

    pclose(gnuplot);
}

void plotBarWErrors(const char *output) {
    char result_file_name[128];
    strcpy(result_file_name, output);
    strcat(result_file_name, "_results");

    FILE *gnuplot = popen("gnuplot", "w");

    const char *output_cmd = "set output \'%s.png\'";
    const char *plot_cmd = "plot \'%s\' using 2:3:4:xticlabels(1) title columnheader";
    char gnuplot_cmds[][128] = {
                        "clear",
                        "reset",
                        "unset key",
                        "set terminal png",
                        "set xtics rotate out",
                        "set style data histogram",
                        "set style histogram errorbars linewidth 1",
                        "set style fill solid 0.3",
                        "set bars front",
                        "",
                        "",
                        "exit"
                        };

    sprintf(gnuplot_cmds[sizeof(gnuplot_cmds)/128 -3], output_cmd, output);
    sprintf(gnuplot_cmds[sizeof(gnuplot_cmds)/128 -2], plot_cmd, result_file_name);

    for (size_t idx = 0 ; idx < (sizeof(gnuplot_cmds)/128) ; idx++) {
        fprintf(gnuplot,"%s\n", gnuplot_cmds[idx]);
        fflush(gnuplot);
    }

    pclose(gnuplot);
}
