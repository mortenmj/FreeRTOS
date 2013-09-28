/*
 * ring_buffer.h
 *
 * Created: 29.08.2013 19:05:26
 *  Author: Atmel
 */ 


#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include <assert.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ring_buffer {
	volatile uint8_t write_offset;
	volatile uint8_t read_offset;
	uint8_t size;
	uint8_t *buffer;
};

static inline uint8_t get_next(uint8_t cur_offset, uint8_t max_offset)
{
	return (cur_offset == max_offset ? 0 : cur_offset + 1);
}

static inline uint8_t ring_buffer_get_next_write(const struct ring_buffer *ring)
{
	return get_next(ring->write_offset, ring->size);
}

static inline uint8_t ring_buffer_get_next_read(const struct ring_buffer *ring)
{
	return get_next(ring->read_offset, ring->size);
}

static inline bool ring_buffer_is_full(const struct ring_buffer *ring)
{
	return (ring->read_offset == ring_buffer_get_next_write(ring));
}

static inline bool ring_buffer_is_empty(const struct ring_buffer *ring)
{
	return (ring->read_offset == ring->write_offset);
}

static inline struct ring_buffer ring_buffer_init(uint8_t *buffer, uint8_t size)
{
	struct ring_buffer ring;
	ring.write_offset = 0;
	ring.read_offset = 0;
	ring.size = size;
	ring.buffer = buffer;
	return ring;
}

static inline uint8_t ring_buffer_get(struct ring_buffer *ring)
{
	//assert(!ring_buffer_is_empty(ring));
	uint8_t data = ring->buffer[ring->read_offset];
	ring->read_offset = ring_buffer_get_next_read(ring);
	return data;
}

static inline void ring_buffer_put(struct ring_buffer *ring, uint8_t data)
{
	//assert(!ring_buffer_is_full(ring));
	ring->buffer[ring->write_offset] = data;
	ring->write_offset = ring_buffer_get_next_write(ring);
}

#ifdef __cplusplus
}
#endif

#endif /* RING_BUFFER_H_ */