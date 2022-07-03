#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "custom_malloc.h"

typedef struct alloc {
	uint32_t magic_value;
	uint16_t is_free;
	uint16_t is_split;
	size_t size;
	struct alloc *next;
	struct alloc *prev;
	uintptr_t data[1];
} alloc_t;

#define MAGICVALUE 0xdeadbeef

#define THRESHOLD (2 * sizeof(alloc_t))

static alloc_t gbl_free_lists = {0};
static alloc_t gbl_reserved_lists = {0};

static size_t get_overall_instance_size(size_t bytes)
{
	size_t new_size = sizeof(alloc_t) + bytes - sizeof(uintptr_t);
	return new_size;
}

static void *get_block_from_instance(void *ptr)
{
	uintptr_t *block_common = NULL;
	alloc_t *block;

	block_common = (uintptr_t *)((uintptr_t)ptr - sizeof(alloc_t) + sizeof(uintptr_t));
	block = (alloc_t *)block_common;

	if (block->magic_value != MAGICVALUE) {
		return NULL;
	}

	return block;
}

static bool block_can_split(alloc_t *block, size_t size)
{
	size_t block_size = block->size;
	size_t block_full_size = get_overall_instance_size(block_size);

	size_t required_curr_size = get_overall_instance_size(size);

	if (required_curr_size + THRESHOLD < block_full_size)
		return true;

	return false;
}

static void insert_into_gbl_list(alloc_t *gbl_list, alloc_t *instance)
{
	alloc_t *last_node = gbl_list->prev;

	last_node->next = instance;
	instance->prev = last_node;
	instance->next = gbl_list;
	gbl_list->prev = instance;
}

static void remove_from_list(alloc_t *gbl_list, alloc_t *instance)
{
	alloc_t *prev = instance->prev;
	alloc_t *next = instance->next;
	prev->next = next;
	next->prev = prev;

#ifdef DEBUG
	fprintf(stdout, "gbl_list: %p, prev: %p, next: %p, curr = %p\n", gbl_list, prev, next, instance->data);
#endif	

	instance->next = instance;
	instance->prev = instance;
}

static void free_node(alloc_t *instance)
{
#ifdef DEBUG
	fprintf(stdout, "struct of size %lu\n", instance->size);
#endif
	instance->is_free = 1;
	memset(instance->data, 0, instance->size);
}

static void alloc_init(alloc_t *instance, size_t size)
{
	instance->magic_value = MAGICVALUE;
	instance->is_free = 0;
	instance->size = size;
	instance->next = instance;
	instance->prev = instance;
	memset(instance->data, 0, size);

#ifdef DEBUG
	fprintf(stdout, "Init alloc struct of size %lu\n", size);
#endif
}

void __attribute__ ((constructor)) premain()
{
#ifdef DEBUG
	fprintf(stdout, "size of alloc_t = %lu\n", sizeof(alloc_t));
#endif

	alloc_init(&gbl_free_lists, sizeof(uintptr_t));
	alloc_init(&gbl_reserved_lists, sizeof(uintptr_t));
}

static void *alloc_system_memory(size_t bytes) {
	void *prev = sbrk(0);
	if (sbrk(bytes) == (void *)-1) {
		fprintf(stderr, "Error allocating system memory\n");
		return NULL;
	}

#ifdef DEBUG
	fprintf(stdout, "Allocating from system-memory\n");
#endif

	return prev;
}

void *find_free_node(size_t bytes) {
	alloc_t *tmp = NULL;
	alloc_t *ret = NULL;
	size_t diff = UINT32_MAX;

#ifdef DEBUG
	printf("[global free list address]: %p \n", &gbl_free_lists);
#endif
	for (tmp = gbl_free_lists.next; tmp != &gbl_free_lists; tmp = tmp->next) {
#ifdef DEBUG
		printf("[address node], [size = %lu]: %p \n", tmp->size, tmp->data);
#endif
		if (bytes <= tmp->size) {
			if (tmp->size - bytes < diff) {
#ifdef DEBUG
				printf("[tmp->data = %p], [tmp->size = %lu], [bytes = %lu], [diff = %lu]: \n", tmp->data, tmp->size, bytes, diff);
#endif
				diff = tmp->size - bytes;
				ret = tmp;
			}
		}
	}

#ifdef DEBUG
	if (ret != NULL)
		fprintf(stdout, "Found already allocated node %p of size %lu\n", ret, ret->size);
#endif

	return ret;
}

static void split_block(alloc_t *block, size_t size)
{
	alloc_t *new_block = (alloc_t *)((uintptr_t)block + sizeof(alloc_t) - sizeof(uintptr_t) + size);
	*new_block = *block;

	new_block->size = block->size - size;
	new_block->prev = block;

	block->size = size;
	block->next = new_block;

	block->is_split = 1;
}

static bool block_has_next_free(alloc_t *block)
{
	size_t size = block->size;
	alloc_t *next = (alloc_t *)((uintptr_t)block + sizeof(alloc_t) - sizeof(uintptr_t) + size);
}

static void join_block(alloc_t *block_prev, alloc_t *block_next)
{
	
}

void *alloc_memory(size_t bytes)
{
	void *ret = NULL;
	size_t required_size = get_overall_instance_size(bytes);
	void *block = NULL;
	alloc_t *block_data;

	if (bytes == 0 || (bytes < sizeof(uintptr_t))) {
		fprintf(stderr, "Allocate a minimum of 8 bytes\n");
		return ret;
	}

#ifdef DEBUG
	fprintf(stdout, "Requesting node of size %lu\n", bytes);
#endif

	block = find_free_node(bytes);

	if (block == NULL) {
#ifdef DEBUG
		fprintf(stdout, "No free nodes of size %lu available\n", bytes);
#endif
		block = alloc_system_memory(required_size);
		if (block == NULL) {
			return ret;
		}
	} else {
		if (block_can_split(block, bytes)) {

		}
		remove_from_list(&gbl_free_lists, block);
	}

	block_data = (alloc_t *)block;
	alloc_init(block_data, bytes);
	ret = block_data->data;

	insert_into_gbl_list(&gbl_reserved_lists, block_data);

	return ret;
}

void free_memory(void *ptr)
{
	alloc_t *block = NULL;
	if (ptr == NULL) {
		return;
	}

	block = get_block_from_instance(ptr);
	if (block == NULL) {
		fprintf(stderr, "Invalid pointer passed to free()\n");
		return;
	}

	free_node(block);

	remove_from_list(&gbl_reserved_lists, block);
	insert_into_gbl_list(&gbl_free_lists, block);

#ifdef DEBUG
	fprintf(stdout, "Freed node of size %lu\n", block->size);
#endif

	return;
}