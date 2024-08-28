#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "api.h"

/************** TYPE DEFINITION ****************/
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


/************** FUNCTION DECLARATION ****************/
T_VOID optimizationTUI();
T_VOID optimizeConfig(PARAM_GRID (*optimizationFunc)(OPT_MAP *, PARAM_GRID, size_t, T_DOUBLE (*)(OPT_MAP *, PARAM_GRID), const char *), \
                                    T_DOUBLE (*objectiveFunc)(OPT_MAP *, PARAM_GRID), \
                                    size_t iterations, const char *output);

#endif
