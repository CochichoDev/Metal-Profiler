#include "plot.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

void uint64_t_saveData(const char *output, ...) {
    char result_file_name[128];
    strcpy(result_file_name, output);
    strcat(result_file_name, "_results");
    
    FILE *output_file = fopen(result_file_name, "w");

    va_list va;
    va_start(va, output);
    
    void *result_ptr;
    fputs("Title\tMedian\tMin\tMax\n", output_file);
    while ((result_ptr = va_arg(va, void*))) {
        RESULTS(uint64_t) *result = result_ptr;
        fprintf(output_file, "%s\t%lu\t%lu\t%lu\n", result->name, result->median, result->min, result->max);
    }
    
    va_end(va);
    fclose(output_file);
}
void double_saveData(const char *output, ...) {
    char result_file_name[128];
    strcpy(result_file_name, output);
    strcat(result_file_name, "_results");
    
    FILE *output_file = fopen(result_file_name, "w");

    va_list va;
    va_start(va, output);
    
    void *result_ptr;
    fputs("Title\tMedian\tMin\tMax\n", output_file);
    while ((result_ptr = va_arg(va, void*)) != NULL) {
        RESULTS(double) *result = result_ptr;
        fprintf(output_file, "%s\t%f\t%f\t%f\n", result->name, result->median, result->min,  result->max);
    }
    
    va_end(va);
    fclose(output_file);
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
