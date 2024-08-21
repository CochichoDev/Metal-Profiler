#include <stdio.h>
#include <stdlib.h>

#include "generics.h"
#include "api.h"
#include "apistate.h"

T_VOID __T_INT__InitializeGeneric(G_ARRAY* gen_data, size_t size) {
    if (!gen_data) {
        fprintf(stderr, "Error: Tried to initialize NULL pointer\n");
        return;
    }

    gen_data->DATA = malloc(sizeof(T_INT) * size);
    gen_data->TYPE = G_INT;
    gen_data->SIZE = size;
}

T_VOID __T_UINT__InitializeGeneric(G_ARRAY* gen_data, size_t size) {
    if (!gen_data) {
        fprintf(stderr, "Error: Tried to initialize NULL pointer\n");
        return;
    }

    gen_data->DATA = malloc(sizeof(T_UINT) * size);
    gen_data->TYPE = G_UINT;
    gen_data->SIZE = size;
}

T_VOID __T_DOUBLE__InitializeGeneric(G_ARRAY* gen_data, size_t size) {
    if (!gen_data) {
        fprintf(stderr, "Error: Tried to initialize NULL pointer\n");
        return;
    }

    gen_data->DATA = malloc(sizeof(T_DOUBLE) * size);
    gen_data->TYPE = G_DOUBLE;
    gen_data->SIZE = size;
}

T_VOID DESTROY_GENERIC(G_ARRAY* gen_data) {
    if (!gen_data) {
        fprintf(stderr, "Error: Tried to destroy a NULL generic\n");
        return;
    }

    if (!gen_data->DATA) {
        fprintf(stderr, "Error: Tried to destroy a non initialized generic\n");
        return;
    }
    free(gen_data->DATA);
}
