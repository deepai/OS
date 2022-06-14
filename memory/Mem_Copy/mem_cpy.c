#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "mem_cpy.h"

#define WORD uintptr_t;
#define ALIGN_WORD sizeof(uintptr_t)

static bool isAligned(void *pointer) {
    if (((uintptr_t)pointer & (ALIGN_WORD - 1)) == 0) {
        return true;
    } else {
        return false;
    }
}

static size_t mem_cpy_backward(void *src, void *dest, size_t bytes)
{
    uintptr_t *dest_uintptr;
    uintptr_t *src_uintptr;
    char *src_byte = src;
    char *dest_byte = dest;
    int i = 0, j = 0;
    int num_copies = 0;

    size_t current_bytes = 0;
    size_t aligned_size = ALIGN_WORD;

    if (isAligned(src_byte + bytes) && isAligned(dest_byte + bytes)) {
#ifdef DEBUG
        fprintf(stderr, "[Debug]: src = %p, dest = %p, Aligned memory, wordsize = %lu\n", src, dest, ALIGN_WORD);
#endif
        dest_uintptr = (uintptr_t *)(dest_byte + bytes);
        src_uintptr = (uintptr_t *)(src_byte + bytes);
        dest_uintptr--;
        src_uintptr--;
        while (current_bytes + aligned_size <= bytes) {
            *dest_uintptr = *src_uintptr;
            dest_uintptr--;
            src_uintptr--;
            current_bytes += aligned_size;
            num_copies++;
        }
    } else {
#ifdef DEBUG
        fprintf(stderr, "[Debug]: src = %p, dest = %p, Non aligned memory, wordsize = %lu\n", src, dest, ALIGN_WORD);
#endif
        dest_byte = dest + bytes - 1;
        src_byte = src + bytes - 1;
        for (; current_bytes + 1 <= bytes; j++) {
            *dest_byte = *src_byte;
            dest_byte--;
            src_byte--;
            current_bytes++;
            num_copies++;
        }
    }

#ifdef DEBUG
        fprintf(stderr, "[Debug]: Number of data copies = %d\n", num_copies);
#endif
    return current_bytes;
}

static size_t mem_cpy_forward(void *src, void *dest, size_t bytes)
{
    uintptr_t *dest_uintptr;
    uintptr_t *src_uintptr;
    char *src_byte = src;
    char *dest_byte = dest;
    int i = 0, j = 0;

    size_t current_bytes = 0;
    size_t aligned_size = ALIGN_WORD;
    int num_copies = 0;

    if (isAligned(src) && isAligned(dest)) {
#ifdef DEBUG
        fprintf(stderr, "[Debug]: src = %p, dest = %p, Aligned memory, wordsize = %lu\n", src, dest, ALIGN_WORD);
#endif
        dest_uintptr = dest;
        src_uintptr = src;
        for (; current_bytes + ALIGN_WORD <= bytes; i++) {
            dest_uintptr[i] = src_uintptr[i];
            current_bytes += ALIGN_WORD;
            j += ALIGN_WORD;
            num_copies++;
        }
    } else {
#ifdef DEBUG
        fprintf(stderr, "[Debug]: src = %p, dest = %p, Non aligned memory, wordsize = %lu\n", src, dest, ALIGN_WORD);
#endif
    }

    for (; current_bytes + 1 <= bytes; j++) {
        dest_byte[j] = src_byte[j];
        current_bytes++;
        num_copies++;
    }

#ifdef DEBUG
        fprintf(stderr, "[Debug]: Number of data copies = %d\n", num_copies);
#endif

    return current_bytes;
}

size_t mem_cpy(void *src, void *dest, size_t bytes)
{
    size_t ret = 0;
    if (src == dest)
        return ret;
    else if ((uintptr_t) src + bytes <= (uintptr_t) dest) {
        ret = mem_cpy_forward(src, dest, bytes);
    } else if ((uintptr_t)dest < (uintptr_t)src) {
        ret = mem_cpy_forward(src, dest, bytes);
    } else {
#ifdef DEBUG
        fprintf(stderr, "[Debug]: Overlapping address, copying backwards\n");
#endif
        ret = mem_cpy_backward(src, dest, bytes);
    }

    return ret;
}
