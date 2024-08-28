#ifndef STATE_H
#define STATE_H

#include "api.h"

/************** STATE TYPES DEFINITION ****************/
typedef struct {
    char name[128];
    char path[256];
} ARCH;

typedef struct {
    ARCH arch[64];
    size_t num;
} ARCH_LIST;

typedef struct {
    char name[128];
    char path[256];
} FCONFIG;

typedef struct {
    FCONFIG config[128];
    size_t num;
    size_t selected;
} FCONFIG_LIST;

#define NUM_OUTPUT_GRAPHS 2
#define NUM_OUTPUT_DATA 2
typedef struct {
    T_STR NAME;
    TYPE  TYPE;
    size_t DATA_SIZE;
    enum {
        SCATTER,
        BARWERROR
    } GRAPH_TYPE;
    enum {
        RAW,
        DEGRADATION
    } DATA_TYPE;
} OUTPUT;

typedef struct s_OUTPUT_LIST {
    OUTPUT *OUT;
    struct s_OUTPUT_LIST *NEXT;
} OUTPUT_LIST;



/************** PRINTING STATE FUNCTIONS ****************/
T_ERROR listArchs();
T_ERROR listConfigs();
T_VOID printConfig();
T_ERROR listState();
T_VOID listOutputTypes();
T_VOID listSelectedOutputOptions();

/************** ADDING/MODIFYING STATE FUNCTIONS ****************/
T_VOID loadAvailableArchs();
T_VOID loadAvailableConfigs();
T_VOID selectArch(size_t choice);
T_VOID loadConfig(T_UINT config_option);
T_ERROR addOutputOption(T_PSTR graph, T_PSTR data, T_PSTR name, size_t size, TYPE type);

/************** DELETING STATE FUNCTIONS ****************/
T_ERROR deleteOutputOption(T_PSTR name);
T_VOID destroyConfig(CONFIG *cfg);
T_ERROR cleanState();

T_ERROR addOutputOption(T_PSTR graph, T_PSTR data, T_PSTR name, size_t size, TYPE type);
T_VOID listSelectedOutputOptions();
T_ERROR deleteOutputOption(T_PSTR name);

T_ERROR cleanState();
T_VOID destroyConfig(CONFIG *cfg);

#endif
