#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "mem_cpy.h"

#define MB_TO_KB(N) ((N) * (1024))

#define WORDSIZE uint32_t
#define COPYSIZE 10

int main(int argc, char **argv) {
    uint32_t i = 0;
    size_t ret = 0;
    void *mem = (WORDSIZE *)malloc(sizeof (WORDSIZE) * MB_TO_KB(500));
    if (mem == NULL) {
        printf("Error allocating memory\n");
        return -1;
    }

    WORDSIZE *storage = (WORDSIZE *)mem;
    for (i = 0; i < MB_TO_KB(100); i++) {
        storage[i] = i;
    }

    ret = mem_cpy(storage + 0, storage + COPYSIZE, sizeof(WORDSIZE) * COPYSIZE);
    printf("Memcpy returned %lu bytes\n", ret);

    for (i = 0; i < 25; i++) {
        printf("mem[%u] = %lu\n", i, (uint64_t)storage[i]);
    }

    ret = mem_cpy(storage, storage + 5, sizeof(WORDSIZE) * COPYSIZE);
    printf("Memcpy returned %lu bytes\n", ret);

    for (i = 0; i < 25; i++) {
        printf("mem[%u] = %lu\n", i, (uint64_t)storage[i]);
    }

    ret = mem_cpy(storage + 8, storage + 16, sizeof(WORDSIZE) * COPYSIZE);
    printf("Memcpy returned %lu bytes\n", ret);

    free(mem);
    return 0;
};
