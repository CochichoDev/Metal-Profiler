/*
 * File: plot.c
 * Plotting related
 * Author: Diogo Cochicho
 */

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "api.h"
#include "global.h"
#include "plot.h"


/************** GENERAL RESULT PLOTTING ****************/
/*
 * plotResults : It belongs to the execution pipeline it profits from the 
 *                  global OUTPUT_LIST to know which graphs to produce and
 *                  what data to plot
 */
T_VOID plotResults(const char *name, size_t num_outputs, OUTPUT **output_array) {
    assert(num_outputs > 0);
    assert(output_array != NULL);
    
    FILE *f_data;

    T_STR selected_data = { 0 };
    T_STR output_name = { 0 };
    for (size_t out_idx = 0; out_idx < num_outputs; out_idx++) {
        switch (output_array[out_idx]->GRAPH_TYPE) {
            case SCATTER:
                strcpy(selected_data, output_array[out_idx]->NAME);
                switch (output_array[out_idx]->DATA_TYPE) {
                    case RAW:
                        strcat(selected_data, "_raw");
                        break;
                    case DEGRADATION:
                        strcat(selected_data, "_deg");
                }

                // HACK: It finds the first time where the raw file was created
                f_data = fopen(selected_data, "r");
                if (!f_data) {
                    perror("Error: ");
                    continue;
                }
                fclose(f_data);
                
                strcpy(output_name, selected_data);
                strcat(output_name, "_scatter");
                plotScatter(selected_data, output_name);


                break;    

            case BARWERROR:
                strcpy(selected_data, output_array[out_idx]->NAME);
                switch (output_array[out_idx]->DATA_TYPE) {
                    case RAW:
                        strcat(selected_data, "_raw");
                        break;
                    case DEGRADATION:
                        strcat(selected_data, "_deg");
                }

                strcpy(output_name, selected_data);
                strcat(selected_data, "_metrics");

                f_data = fopen(selected_data, "r");
                if (!f_data) {
                    perror("Error: ");
                    continue;
                }
                fclose(f_data);
                
                strcat(output_name, "_barwerror");
                plotBarWErrors(selected_data, output_name);
        }
    }
}

/************** TYPES OF PLOTS ****************/
T_VOID plotScatter(const T_PSTR input, const T_PSTR output) {
    FILE *gnuplot = popen("gnuplot", "w");

    const T_PSTR output_cmd = "set output \'%s.png\'";
    const T_PSTR plot_cmd = "plot for [col=2:*:1] '%s' using 1:col with lines title columnheader";
    char gnuplot_cmds[][128] = {
                        "clear",
                        "reset",
                        "set terminal png",
                        "",
                        "",
                        "exit"
                        };

    sprintf(gnuplot_cmds[sizeof(gnuplot_cmds)/128 -3], output_cmd, output);
    sprintf(gnuplot_cmds[sizeof(gnuplot_cmds)/128 -2], plot_cmd, input);

    puts(plot_cmd);

    for (size_t idx = 0 ; idx < (sizeof(gnuplot_cmds)/128) ; idx++) {
        fprintf(gnuplot,"%s\n", gnuplot_cmds[idx]);
        fflush(gnuplot);
    }

    pclose(gnuplot);
}

T_VOID plotBarWErrors(const T_PSTR input, const T_PSTR output) {
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
    sprintf(gnuplot_cmds[sizeof(gnuplot_cmds)/128 -2], plot_cmd, input);

    for (size_t idx = 0 ; idx < (sizeof(gnuplot_cmds)/128) ; idx++) {
        fprintf(gnuplot,"%s\n", gnuplot_cmds[idx]);
        fflush(gnuplot);
    }

    pclose(gnuplot);
}
