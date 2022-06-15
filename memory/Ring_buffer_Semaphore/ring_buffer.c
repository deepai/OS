#include <string.h>
#include <stdio.h>
#include "ring_buffer.h"
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

struct thread {
	pthread_t thread;
	int id;
	int consumed_items;
	sem_t sem_full;
	sem_t sem_empty;
};

typedef struct thread thread_t;

struct ring_buffer_t {
	void *buffer;
	uint32_t head;
	uint32_t tail;
	uint32_t size;
	thread_t consumer;
	volatile bool exit;
};

static void *consumer_thread(void *arg);

//private methods

static void init_thread(thread_t *t, ring_buffer *buf)
{
	t->consumed_items = 0;
	t->id = pthread_create(&t->thread, NULL, consumer_thread, buf);
	sem_init(&t->sem_empty, 0, 1);
	sem_init(&t->sem_full, 0, 0);
}

static void destroy_thread(thread_t *t)
{
	pthread_cancel(t->thread);
	pthread_join(t->thread, NULL);

	printf("Deinit Ring Buffer Consumer Thread. Total processed items = %d\n", t->consumed_items);
}

static void ring_buffer_clear(ring_buffer *buf)
{
	buf->head = 0;
	buf->tail = 0;
	buf->exit = false;
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

void *consumer_thread(void *arg) {
	ring_buffer *buf = (ring_buffer *)arg;
	thread_t *consumer = &buf->consumer;
	uint32_t *store = buf->buffer;

	while (1) {
		sem_wait(&consumer->sem_full);

		while (!ring_buffer_empty(buf)) {
			uint32_t data = store[buf->tail];
			advance_pointer(buf, &buf->tail, buf->size);
#ifdef DEBUG
			printf("[Consumer]: Get Data = %u\n", data);
#endif
			consumer->consumed_items++;
		}

#ifdef DEBUG
		printf("[Consumer]: Wakeup producer\n");
#endif

		sem_post(&consumer->sem_empty);
	}


#ifdef DEBUG
		printf("[: consumed items = %u\n", consumer->consumed_items);
#endif

	return &consumer->consumed_items;
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
	init_thread(&buf->consumer, buf);
}

void ring_buffer_deinit(ring_buffer *buf)
{
	thread_t *t = &buf->consumer;

	/* wait on empty */
	sem_wait(&t->sem_empty);

	/* wait on consumer thread to set empty */
	sem_post(&t->sem_full);
#ifdef DEBUG
		printf("[Producer]: Wakeup consumer\n");
#endif

	/* wait on empty again */
	sem_wait(&t->sem_empty);
	
	destroy_thread(&buf->consumer);

	buf->buffer = NULL;
	buf->size = 0;
	ring_buffer_clear(buf);
}

int ring_buffer_put(ring_buffer *buf, uint32_t data)
{
	uint32_t *buffer = (uint32_t *)buf->buffer;
	if (!ring_buffer_full(buf)) {
		buffer[buf->head] = data;
		advance_pointer(buf, &buf->head, buf->size);
		return 0;
	} else {
		return -1;
	}
}

int ring_buffer_get(ring_buffer *buf, uint32_t *data)
{
	uint32_t *store = (uint32_t *)buf->buffer;
	if (!ring_buffer_empty(buf)) {
		*data = store[buf->tail];
		advance_pointer(buf, &buf->tail, buf->size);
		return 0;
	} else {
		return -1;
	}
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

int ring_buffer_put_ts(ring_buffer *buf, uint32_t data)
{
	thread_t *consumer = &buf->consumer;
	uint32_t *store = buf->buffer;

	/* Start with empty */
	sem_wait(&consumer->sem_empty);

	store[buf->head] = data;
	advance_pointer(buf, &buf->head, buf->size);
#ifdef DEBUG
	printf("[Producer]: Put Data = %u\n", data);
#endif

	if (ring_buffer_full(buf)) {
#ifdef DEBUG
		printf("[Producer]: Wakeup consumer\n");
#endif
		/* Wait on full */
		sem_post(&consumer->sem_full);
	} else {
		/* Increment empty */
		sem_post(&consumer->sem_empty);
	}
}