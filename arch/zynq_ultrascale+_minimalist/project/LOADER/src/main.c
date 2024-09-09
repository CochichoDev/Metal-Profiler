#include <stdint.h>
#include <stdio.h>

void loader(uintptr_t vma, uintptr_t lma, uint64_t size) {
    for (size_t byte_idx = 0; byte_idx < size; byte_idx++) {
        *( ((volatile uint8_t *) vma) + byte_idx) = *( ((volatile uint8_t *) lma) + byte_idx);
    }
}

