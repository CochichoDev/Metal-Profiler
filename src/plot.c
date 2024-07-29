#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "plot.h"
#include "api.h"
#include "global.h"


/*
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
*/

/*
 * plotResults : It belongs to the execution pipeline it profits from the 
 *                  global OUTPUT_LIST to know which graphs to produce and
 *                  what data to plot
 */
T_VOID plotResults() {
    OUTPUT_LIST *iter = OUTPUT_LIST_SELECTED;
    OUTPUT_LIST *iter_aux;
    
    FILE *f_data;

    T_STR selected_data = { 0 };
    T_STR output_name = { 0 };
    while (iter != NULL) {
        iter_aux = OUTPUT_LIST_SELECTED;

        switch (iter->OUT->GRAPH_TYPE) {
            case SCATTER:
                while (iter_aux != NULL) {
                    if (iter_aux->OUT->DATA_TYPE == iter->OUT->DATA_TYPE) break;
                    iter_aux = iter_aux->NEXT;
                } 

                strcpy(output_name, iter->OUT->NAME);
                strcpy(selected_data, iter_aux->OUT->NAME);
                switch (iter->OUT->DATA_TYPE) {
                    case RAW:
                        strcat(selected_data, "_raw");
                        strcat(output_name, "_raw");
                        break;
                    case DEGRADATION:
                        strcat(selected_data, "_deg");
                        strcat(output_name, "_deg");
                }

                // HACK: It finds the first time where the raw file was created
                f_data = fopen(selected_data, "r");
                if (!f_data) {
                    perror("Error: ");
                    continue;
                }
                fclose(f_data);
                
                strcat(output_name, "_scatter");
                plotScatter(selected_data, output_name);


                break;    

            case BARWERROR:
                while (iter_aux != NULL) {
                    if (iter_aux->OUT->DATA_TYPE == iter->OUT->DATA_TYPE) break;
                    iter_aux = iter_aux->NEXT;
                } 

                strcpy(output_name, iter->OUT->NAME);
                strcpy(selected_data, iter_aux->OUT->NAME);
                switch (iter->OUT->DATA_TYPE) {
                    case RAW:
                        strcat(selected_data, "_raw");
                        strcat(output_name, "_raw");
                        break;
                    case DEGRADATION:
                        strcat(selected_data, "_deg");
                        strcat(output_name, "_deg");
                }

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

        iter = iter->NEXT;
    }
}

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
