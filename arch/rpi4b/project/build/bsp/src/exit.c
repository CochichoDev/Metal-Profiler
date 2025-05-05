#include "common.h"

void exit(s32 code) {
    ((void (*)()) 0x900)();
}
