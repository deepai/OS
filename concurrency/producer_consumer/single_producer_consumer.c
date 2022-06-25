#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#include "ring_buffer.h"

struct thread_shared_resource {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    struct ring_buffer_t *r_buf;
};

typedef struct thread_shared_resource thread_shared_res_t;

struct producer_thread_data {
    pthread_t thread;
    uint32_t num_items_to_produce;
    thread_shared_res_t *resource;
};

struct consumer_thread_data {
    uint32_t num_items_consumed;
    pthread_t thread;
    thread_shared_res_t *resource;
};

static void init_shared_thread_data(thread_shared_res_t *res, struct ring_buffer_t *r_buf)
{
    pthread_mutex_init(&res->mutex, NULL);
    pthread_cond_init(&res->cond, NULL);
    res->r_buf = r_buf;
}

static void destroy_shared_thread_data(thread_shared_res_t *res)
{
    pthread_mutex_destroy(&res->mutex);
    pthread_cond_destroy(&res->cond);

    res->r_buf = NULL;
}


static void *producer_thread(void *arg) {
    struct producer_thread_data *data = (struct producer_thread_data *)arg;
    thread_shared_res_t *res = data->resource;
    struct ring_buffer_t *r_buf = res->r_buf;

    srand(0);

    fprintf(stdout, "started producer thread\n");

    for (uint32_t i = 0; i < data->num_items_to_produce; i++) {
        uint32_t value;

        pthread_mutex_lock(&res->mutex);

        if (ring_buffer_full(r_buf)) {
            fprintf(stdout, "buffer is full! producer thread going to sleep.\n");
            pthread_cond_wait(&res->cond, &res->mutex);
        }

        value = (uint32_t)(rand() % (UINT32_MAX));
        ring_buffer_put(r_buf, value);

        fprintf(stdout, "producer thread finished work [%u].\n", i);

        if (ring_buffer_full(r_buf)) {
            pthread_cond_signal(&res->cond);
        }

        pthread_mutex_unlock(&res->mutex);
    }
}

static void *consumer_thread(void *arg) {
    struct consumer_thread_data *data = (struct consumer_thread_data *)arg;
    thread_shared_res_t *res = data->resource;
    struct ring_buffer_t *r_buf = res->r_buf;

    fprintf(stdout, "started consumer thread\n");

    while (1) {
        uint32_t value;

        pthread_mutex_lock(&res->mutex);

        if (ring_buffer_empty(r_buf)) {
            fprintf(stdout, "buffer is empty! consumer thread going to sleep.\n");
            pthread_cond_wait(&res->cond, &res->mutex);
        }

        fprintf(stdout, "consumer thread wake up.\n");

        while (!ring_buffer_empty(r_buf)) {
            ring_buffer_get(r_buf, &value);
            data->num_items_consumed++;
            fprintf(stdout, "consumer thread processed work [%u].\n",  data->num_items_consumed - 1);
        }

        fprintf(stdout, "consumer thread processed all values.\n");

        pthread_cond_signal(&res->cond);

        pthread_mutex_unlock(&res->mutex);
    }
}

static int init_producer(struct producer_thread_data *data, thread_shared_res_t *res, uint32_t num_items) {
    int err;
    data->num_items_to_produce = num_items;
    data->resource = res;
    err = pthread_create(&data->thread, NULL, producer_thread, data);
    if (err != 0) {
        fprintf(stderr, "Unable to create producer thread\n");
    }

    return err;
}

static int init_consumer(struct consumer_thread_data *data, thread_shared_res_t *res) {
    int err;
    data->num_items_consumed = 0;
    data->resource = res;
    err = pthread_create(&data->thread, NULL, consumer_thread, data);
    if (err != 0) {
        fprintf(stderr, "Unable to create consumer thread\n");
    }

    return err;
}

int main(int argc, char **argv) {
    uint32_t num_buffer_elements;
    uint32_t num_produced_items;
    struct producer_thread_data producer_data = {0};
    struct consumer_thread_data consumer_data = {0};
    thread_shared_res_t shared_resource = {0};

    struct ring_buffer_t *ring_buffer;

	if (argc < 3 || argc > 3) {
		fprintf(stderr, "The correct format is ./single_producer_consumer <num_buffer_elements> <num_produced_items>\n");
		return -1;
	}

    num_buffer_elements = atoi(argv[1]);
    if (num_buffer_elements == 0) {
        fprintf(stderr, "buffer size must be greater than zero.\n");
        return -1;
    }

    num_produced_items = atoi(argv[2]);

    uint32_t *backing_buffer = (uint32_t *)malloc(sizeof(int) * num_buffer_elements);
    if (backing_buffer == NULL) {
        fprintf(stderr, "Unable to allocate backing buffer memory.\n");
        return -1;
    }

    ring_buffer = create_buffer();
    ring_buffer_init(ring_buffer, backing_buffer, num_buffer_elements);

    init_shared_thread_data(&shared_resource, ring_buffer);

    init_consumer(&consumer_data, &shared_resource);
    init_producer(&producer_data, &shared_resource, num_produced_items);

    pthread_join(producer_data.thread, NULL);

    pthread_mutex_lock(&shared_resource.mutex);
    if (!ring_buffer_empty(shared_resource.r_buf)) {
        pthread_cond_signal(&shared_resource.cond);
        pthread_cond_wait(&shared_resource.cond, &shared_resource.mutex);
    }
    pthread_mutex_unlock(&shared_resource.mutex);

    pthread_cancel(consumer_data.thread);
    pthread_join(consumer_data.thread, NULL);

    destroy_shared_thread_data(&shared_resource);

    free(backing_buffer);

    return 0;
}