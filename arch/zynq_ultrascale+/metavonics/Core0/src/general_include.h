
// Include generaux 

#ifndef GENERAL_INCLUDE
#define GENERAL_INCLUDE
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Choix de la source de temps 
#define PMU 1     
#define CLOCK  2  
#define TIMEOFDAY 3 
#define TIMECHOICE PMU

// Victime et agresseur
#define AGGRESSOR 1
#define VICTIM 0

// Nombre de fois ou un bench est repete
#ifndef REPEAT
#define REPEAT 10
#endif

#endif

