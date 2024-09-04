#include <stdint.h>
#include <stdio.h>

int global = 0xCU;

int main() {
    printf("%d\n", global);
    return 0;
}

