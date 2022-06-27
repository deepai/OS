#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "aligned_mem.h"

#define ARR_SIZE 10

static bool check_align_addr_correctness(void *ptr, size_t align_bytes)
{
	uintptr_t address = (uintptr_t)ptr;
	if ((address & (align_bytes - 1)) == 0)
		return true;
	else
		return false;
}

int main(int argc, char **argv) {
	int i = 0;

	if (argc != 3) {
		fprintf(stdout, "1st argument is size in bytes\n");
		fprintf(stdout, "1st argument is aligned_size in bytes\n");
		return -1;
	}

	int size = atoi(argv[1]);
	int aligned_size = atoi(argv[2]);
	if (size <= 0 || aligned_size <= 0) {
		fprintf(stderr, "size and aligned_size must be > 0 bytes\n");
	}

	void **arr_of_ptrs = NULL;

	arr_of_ptrs = malloc(sizeof(uintptr_t) * ARR_SIZE);
	if (arr_of_ptrs == NULL) {
		fprintf(stderr, "Unable to allocate array for storing pointers\n");
		return 0;
	}

	for (i = 0; i < ARR_SIZE; i++) {
		arr_of_ptrs[i] = NULL;
		arr_of_ptrs[i] = malloc_aligned((size_t)size, (size_t)aligned_size);
		if (arr_of_ptrs[i] == NULL) {
			fprintf(stderr, "Unable to allocate pointer:[%d]\n", i);
			break;
		} else if (!check_align_addr_correctness(arr_of_ptrs[i], aligned_size)) {
			fprintf(stderr, "Allocated address is not aligned\n");
		}
		fprintf(stdout, "Pointer allocated is %p\n", arr_of_ptrs[i]);
	}

	i--;

	for (; i >= 0; i--) {
		fprintf(stdout, "Freeing Pointer: %p\n", arr_of_ptrs[i]);
		mem_align_free(arr_of_ptrs[i], aligned_size);
		arr_of_ptrs[i] = NULL;
	}

	free(arr_of_ptrs);

	return 0;
};