#include <stdio.h>
#include "ring_buffer.h"
#include <stdint.h>
#include <stdlib.h>

void *createHeapMemory(size_t size, size_t datatypesize) {
	void *data = (void *)malloc(size * datatypesize);
	return data;
}

void freeHeapMemory(void *data) {
	free(data);
}

int main(int argc, char **argv) {

	uint32_t size = 0;
	uint32_t num_elements = 0;
	struct ring_buffer_t *rbuf = NULL;
	int ret = 0;
	uint32_t data = 0;

	if (argc < 3 || argc > 4) {
		printf("The correct format is ./ring_buffer <size> <num_elements>\n");
		return -1;
	}

	size = atoi(argv[1]);
	num_elements = atoi(argv[2]);

	printf("Created Ring Buffer[Maxsize: %u]\n", size);

	void *buffer_memory = createHeapMemory(size, sizeof(uint32_t));
	if (!buffer_memory) {
		printf("Error allocating memory\n");
	}

	rbuf = create_buffer();
	ring_buffer_init(rbuf, buffer_memory, size);

	for (uint32_t i = 0; i < num_elements; i++) {
		ret = ring_buffer_put_ts(rbuf, i);
		if (ret != 0) {
			printf("Unable to add element %u, as the buffer is full\n", i);
		}
	}

	ring_buffer_deinit(rbuf);
	freeHeapMemory(buffer_memory);

	return 0;
}