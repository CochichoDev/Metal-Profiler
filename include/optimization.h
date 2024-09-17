#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include <stdio.h>

#include "api.h"
#include "types.h"

/************** TYPE DEFINITION ****************/
typedef struct {
    COMP        *ROWS[16];
    PROP        **PROPS[16];
    T_UCHAR     PROPS_P_ROW[16];
    T_UCHAR     NUM_COMP;
    CONFIG      *CFG;
} OPT_MAP;

typedef struct {
    size_t max;
    size_t cur;
} PARAM_ELEM;

typedef PARAM_ELEM *PARAM_ROW;
typedef PARAM_ROW *PARAM_GRID;

typedef struct {
    T_DOUBLE    DEG;
    PARAM_GRID  GRID;
} OPT_RESULT;


/************** FUNCTION DECLARATION ****************/
T_VOID optimizationTUI();
T_VOID printParameterGrid(T_INT descriptor, OPT_MAP *mapGrid, PARAM_GRID grid);
T_VOID printParameterGridFILE(FILE *file, OPT_MAP *mapGrid, PARAM_GRID grid);
T_VOID optimizeConfig(PARAM_GRID (*optimizationFunc)(OPT_MAP *, PARAM_GRID, size_t, G_ARRAY *(*)(OPT_MAP *, PARAM_GRID), const char *), \
                                    G_ARRAY *(*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                                    size_t iterations, const char *output);

#endif
