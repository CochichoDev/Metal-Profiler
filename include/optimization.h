#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "api.h"

typedef struct {
    COMP        *ROWS[16];
    PROP        **PROPS[16];
    T_UCHAR     PROPS_P_ROW[16];
    T_UCHAR     NUM_COMP;
} OPT_MAP;

typedef struct {
    size_t max;
    size_t cur;
} PARAM_ELEM;

typedef PARAM_ELEM *PARAM_ROW;
typedef PARAM_ROW *PARAM_GRID;

T_VOID optimizeConfig(PARAM_GRID (*optimizationFunc)(OPT_MAP *, PARAM_GRID, size_t), size_t iterations);
T_VOID mapConfigToGrid(OPT_MAP *mapGrid);
T_VOID destroMapGrid(OPT_MAP *mapGrid);
PARAM_GRID generateParameterGrid(OPT_MAP *mapGrid);
T_VOID destroyParameterGrid(OPT_MAP *mapGrid, PARAM_GRID grid);
T_VOID printParameterGrid(OPT_MAP *mapGrid, PARAM_GRID grid);
T_DOUBLE objectiveFunc(OPT_MAP *mapGrid, PARAM_GRID param);
PARAM_GRID cloneParams(OPT_MAP *mapGrid, PARAM_GRID param);
PARAM_GRID randomSearch(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations);
PARAM_GRID randomSearchNR(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations);
PARAM_GRID simulatedAnnealing(OPT_MAP *mapGrid, PARAM_GRID param, size_t iterations);
T_INT uniformRandom(T_INT min, T_INT max);
T_INT binomialRandom(T_UINT n, T_DOUBLE p);

#endif
