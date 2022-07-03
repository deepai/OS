#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "custom_malloc.h"

int main(int argc, char *argv[]) {
	int per_allocation_bytes = 0;
	int num_allocation = 0;
	int i = 0, j = 5;

	void **ptr;

	if (argc != 3) {
		fprintf(stderr, "1st argument is size in bytes\n");
		fprintf(stderr, "1st argument is num_allocations\n");
		return -1;
	}

	per_allocation_bytes = atoi(argv[1]);
	num_allocation = atoi(argv[2]);

	if (num_allocation == 0) {
		fprintf(stderr, "2nd argument is greater than 0\n");
	}

	ptr = (void **)(malloc(sizeof(uintptr_t) * num_allocation));
	if (ptr == NULL) {
		fprintf(stderr, "Unable to create memory array\n");
		return -1;
	}

	while (j != 0) {
		i = 0;
		fprintf(stdout, "\n");
		fprintf(stdout, "Iteration [%d]\n", 5 - j + 1);
		size_t size;
		for (; i < num_allocation; i++) {
			ptr[i] = NULL;
			size = rand() % (per_allocation_bytes - 8) + 8;
			ptr[i] = alloc_memory(size);
			if (ptr[i] == NULL) {
				fprintf(stderr, "Error allocating ptr[%d] of size = [%lu] \n", i, size);
				break;
			} else {
				fprintf(stdout, "Allocated ptr[%d] = %p of size = [%lu]\n", i, ptr[i], size);
			}
		}

		i--;
		while (i >= 0) {
			fprintf(stdout, "Freeing ptr[%d] = %p\n", i, ptr[i]);
			free_memory(ptr[i]);
			ptr[i] = NULL;
			i--;
		}

		j--;
	}

	free(ptr);

	return 0;
}