#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "aligned_mem.h"

#define ALIGN_UP(address, align_bytes) ((address + align_bytes - 1) & (~(align_bytes - 1)))

static bool check_align_bytes_correctness(size_t align_bytes)
{
	if (align_bytes == 0) {
		return false;
	}

	/*check for power of 2*/
	if ((align_bytes & (align_bytes - 1)) != 0) {
		fprintf(stderr, "alignment is [%lu] bytes, needs to be a power of 2.\n",align_bytes);
		return false;
	}

	if (align_bytes < sizeof(uintptr_t)) {
		fprintf(stderr, "minimum size for alignment is [%lu] bytes\n", sizeof(uintptr_t));
		return false;
	}

	return true;
}

/**
 * @brief Allocate aligned memory
 * 
 * @param size must be > 0 bytes
 * @param align_bytes must be a power of 2 bytes
 * @return void* NULL if error
 */
void *malloc_aligned(size_t size, size_t align_bytes)
{
	size_t alloc_size;
	void *mem;
	void *ret = NULL;
	uint8_t *byte;

	if (!size) {
		return NULL;
	}

	if (!check_align_bytes_correctness(align_bytes)) {
		fprintf(stderr, "Invalid argument align_size: [%lu]\n", align_bytes);
		return NULL;
	}
	
	alloc_size = sizeof(uintptr_t) + size + align_bytes - 1;

	mem = malloc(alloc_size);
	if (mem == NULL) {
		fprintf(stderr, "Failed to allocate memory\n");
		return NULL;
	}

	ret = (void *)ALIGN_UP((uintptr_t)mem + sizeof(uintptr_t), align_bytes);
	//store the difference
	*((uintptr_t *)ret - 1) = (uintptr_t)ret - (uintptr_t)mem;

	return ret;
}
int mem_align_free(void *ptr, size_t align_bytes)
{
	uintptr_t *offset_store;
	uintptr_t offset_data;
	uintptr_t *original_ptr;

	if (!check_align_bytes_correctness(align_bytes)) {
		fprintf(stderr, "Invalid argument align_size: [%lu]\n", align_bytes);
		return -1;
	}

	offset_store = (uintptr_t *)ptr - 1;
	offset_data = *offset_store;
	original_ptr = (uintptr_t *)((uintptr_t)ptr - offset_data);

	free(original_ptr);

	return 0;
}

