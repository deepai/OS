#ifndef RING_BUFFER_H
#define RING_BUFFER_H
#include <stdint.h>
#include <stdbool.h>

struct ring_buffer_t;
typedef struct ring_buffer_t ring_buffer;

struct ring_buffer_t *create_buffer(void);
void ring_buffer_init(ring_buffer *buf, void *init_buffer, uint32_t size);
void ring_buffer_deinit(ring_buffer *buf);
int ring_buffer_put(ring_buffer *buf, uint32_t data);
int ring_buffer_get(ring_buffer *buf, uint32_t *data);
uint32_t ring_buffer_capacity(ring_buffer *buf);
bool ring_buffer_empty(ring_buffer *buf);
bool ring_buffer_full(ring_buffer *buf);

#endif