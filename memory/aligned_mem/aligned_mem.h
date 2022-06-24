#ifndef _ALIGNED_MEM_H
#define _ALIGNED_MEM_H

#include <stddef.h>

/* aligned bytes must be a power of 2 */
void *malloc_aligned(size_t size, size_t align_bytes);
int mem_align_free(void *ptr, size_t align_bytes);

#endif