#include <string.h>
#include <stdio.h>
#include "ring_buffer.h"
#include <stdlib.h>

struct ring_buffer_t {
	void *buffer;
	uint32_t head;
	uint32_t tail;
	uint32_t size;
};

//private methods

static void ring_buffer_clear(ring_buffer *buf)
{
	buf->head = 0;
	buf->tail = 0;
}

static void construct_ring_buffer(ring_buffer *buf)
{
	buf->buffer = NULL;
	buf->size = 0;
	ring_buffer_clear(buf);
}

static void advance_pointer(ring_buffer *rbuf, uint32_t *pointer, uint32_t max_size)
{
	uint32_t new_pos = ++(*pointer);
	if (new_pos == max_size)
		*pointer = 0;

// #ifdef DEBUG
// 	printf("[: Put = %u, Get = %u\n", rbuf->head, rbuf->tail);
// #endif
}

//public methods

struct ring_buffer_t *create_buffer(void)
{
	static struct ring_buffer_t buffer = { 0 };
	construct_ring_buffer(&buffer);

	return &buffer;
}

void ring_buffer_init(ring_buffer *buf, void *init_buffer, uint32_t size)
{
	buf->buffer = init_buffer;
	buf->size = size;
	ring_buffer_clear(buf);
}

void ring_buffer_deinit(ring_buffer *buf)
{
	buf->buffer = NULL;
	buf->size = 0;
	ring_buffer_clear(buf);
}

int ring_buffer_put(ring_buffer *buf, uint32_t data)
{
	uint32_t *buffer = (uint32_t *)buf->buffer;
	buffer[buf->head] = data;
	advance_pointer(buf, &buf->head, buf->size);
}

int ring_buffer_get(ring_buffer *buf, uint32_t *data)
{
	uint32_t *store = (uint32_t *)buf->buffer;
	*data = store[buf->tail];
	advance_pointer(buf, &buf->tail, buf->size);
}

uint32_t ring_buffer_capacity(ring_buffer *buf)
{
	return buf->size;
}

bool ring_buffer_empty(ring_buffer *buf)
{
	return buf->head == buf->tail;
}

bool ring_buffer_full(ring_buffer *buf)
{
	int head = buf->head;
	int tail = buf->tail;

	if ((head + 1 == tail) || (head == buf->size - 1 && tail == 0))
		return true;
	else
		return false;
}