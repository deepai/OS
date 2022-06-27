#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#include "ring_buffer.h"

struct thread_shared_resource {
    pthread_mutex_t mutex;
    pthread_cond_t cond_full;
    pthread_cond_t cond_not_full;
    struct ring_buffer_t *r_buf;
};

typedef struct thread_shared_resource thread_shared_res_t;

struct producer_thread_data {
    uint id;
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
    pthread_cond_init(&res->cond_full, NULL);
    pthread_cond_init(&res->cond_not_full, NULL);
    res->r_buf = r_buf;
}

static void destroy_shared_thread_data(thread_shared_res_t *res)
{
    pthread_mutex_destroy(&res->mutex);
    pthread_cond_destroy(&res->cond_full);
    pthread_cond_destroy(&res->cond_not_full);
    
    res->r_buf = NULL;
}


static void *producer_thread(void *arg) {
    struct producer_thread_data *data = (struct producer_thread_data *)arg;
    thread_shared_res_t *res = data->resource;
    struct ring_buffer_t *r_buf = res->r_buf;

    srand(0);

    fprintf(stdout, "started producer thread[%u]\n", data->id);

    for (uint32_t i = 0; i < data->num_items_to_produce; i++) {
        uint32_t value;

        pthread_mutex_lock(&res->mutex);

        while (ring_buffer_full(r_buf)) {
            fprintf(stdout, "buffer is full! producer thread[%u] going to sleep.\n", data->id);
            pthread_cond_wait(&res->cond_not_full, &res->mutex);
        }

        value = (uint32_t)(rand() % (UINT32_MAX));
        ring_buffer_put(r_buf, value);

        fprintf(stdout, "producer thread[%u] finished work [%u].\n", data->id, i);

        if (ring_buffer_full(r_buf)) {
            pthread_cond_signal(&res->cond_full);
        }

        pthread_mutex_unlock(&res->mutex);
    }

    fprintf(stdout, "producer thread[%u] finished all work [%u].\n", data->id, data->num_items_to_produce);
}

static void *consumer_thread(void *arg) {
    struct consumer_thread_data *data = (struct consumer_thread_data *)arg;
    thread_shared_res_t *res = data->resource;
    struct ring_buffer_t *r_buf = res->r_buf;

    fprintf(stdout, "started consumer thread\n");

    pthread_mutex_lock(&res->mutex);

    while (1) {
        uint32_t value;

        if (!ring_buffer_full(r_buf)) {
            fprintf(stdout, "buffer is empty! consumer thread going to sleep.\n");
            pthread_cond_wait(&res->cond_full, &res->mutex);
        }

        fprintf(stdout, "consumer thread wake up.\n");

        while (!ring_buffer_empty(r_buf)) {
            ring_buffer_get(r_buf, &value);
            data->num_items_consumed++;
            fprintf(stdout, "consumer thread processed work [%u].\n",  data->num_items_consumed - 1);
        }

        fprintf(stdout, "consumer thread processed all values.\n");

        pthread_cond_broadcast(&res->cond_not_full);
    }

    pthread_mutex_unlock(&res->mutex);
}

static int init_producer(uint i, struct producer_thread_data *data, thread_shared_res_t *res, uint32_t num_items) {
    int err;
    data->id = i;
    data->num_items_to_produce = num_items;
    data->resource = res;
    err = pthread_create(&data->thread, NULL, producer_thread, data);
    if (err != 0) {
        fprintf(stderr, "Unable to create producer [%u]\n", i);
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
    uint32_t num_producers;
    struct producer_thread_data *producer_data;
    struct consumer_thread_data consumer_data = {0};
    thread_shared_res_t shared_resource = {0};

    struct ring_buffer_t *ring_buffer;

	if (argc < 4 || argc > 4) {
		fprintf(stderr, "The correct format is ./single_producer_consumer <num_buffer_elements> <num_producers> <num_produced_items_per_producer>\n");
		return -1;
	}

    num_buffer_elements = atoi(argv[1]);
    if (num_buffer_elements == 0) {
        fprintf(stderr, "buffer size must be greater than zero.\n");
        return -1;
    }

    num_producers = atoi(argv[2]);
    if (num_producers < 2) {
        fprintf(stderr, "number of producers must atleast be 2.\n");
        return -1; 
    }

    num_produced_items = atoi(argv[3]);

    uint32_t *backing_buffer = (uint32_t *)malloc(sizeof(int) * num_buffer_elements);
    if (backing_buffer == NULL) {
        fprintf(stderr, "Unable to allocate backing buffer memory.\n");
        return -1;
    }

    producer_data = (struct producer_thread_data *)calloc(sizeof(struct producer_thread_data), num_producers);
    if (producer_data == NULL) {
        free(backing_buffer);
        fprintf(stderr, "Unable to allocate producers.\n");
        return -1;        
    }

    ring_buffer = create_buffer();
    ring_buffer_init(ring_buffer, backing_buffer, num_buffer_elements);

    init_shared_thread_data(&shared_resource, ring_buffer);

    init_consumer(&consumer_data, &shared_resource);

    for (uint32_t i = 0; i < num_producers; i++) {
        if (init_producer(i, &producer_data[i], &shared_resource, num_produced_items) != 0) {
            fprintf(stderr, "unable to init producer thread. Quitting\n");
            free(producer_data);
            free(backing_buffer);
            return -1;
        }
    }

    for (uint32_t i = 0; i < num_producers; i++) {
        pthread_join(producer_data[i].thread, NULL);
    }

    pthread_mutex_lock(&shared_resource.mutex);
    /* Wait for the consumer thread to finish the last work */
    if (!ring_buffer_empty(shared_resource.r_buf)) {
        pthread_cond_signal(&shared_resource.cond_full);
        /*wait for the consumer thread to signal completition of work */
        pthread_cond_wait(&shared_resource.cond_not_full, &shared_resource.mutex);
    }
    pthread_mutex_unlock(&shared_resource.mutex);

    fprintf(stdout, "Total data consumed by consumer = [%u]\n", consumer_data.num_items_consumed);

    pthread_cancel(consumer_data.thread);
    pthread_join(consumer_data.thread, NULL);

    destroy_shared_thread_data(&shared_resource);

    free(producer_data);
    free(backing_buffer);

    ring_buffer_deinit(ring_buffer);

    return 0;
}