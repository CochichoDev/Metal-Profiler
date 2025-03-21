#include "elf_reader.h"

void deployFirmware(const char *path) {
#ifdef DEBUG
    printf("Deploying firmware %s...\n", path);
#endif 
    open_elf(path, 0);
#ifdef DEBUG
    printf("Finished deploying firmware\n");
#endif 
}
