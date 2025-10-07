#ifndef __RINGBUFFER_INTERFACE__
#define __RINGBUFFER_INTERFACE__

#include <unistd.h>

#define ENQUEUE(handle, ptr)                                \
    ringbuffer_enqueue(handle, ptr, sizeof(*ptr))

#define POP(handle, ptr)                                    \
    ringbuffer_pop(handle, ptr, sizeof(*ptr))

#define LENGTH(handle, type)                                \
    (ringbuffer_size(handle) / sizeof(type))

#define CAPACITY(handle, type)                              \
    (ringbuffer_capacity(handle) / sizeof(type))
    
#ifdef __cplusplus
extern "C" {
#endif

struct ringbuffer;

typedef struct ringbuffer *ringbuffer_handle;

ringbuffer_handle ringbuffer_create(size_t minimum_count, size_t elem_size);

void ringbuffer_free(ringbuffer_handle handle);

bool ringbuffer_enqueue(ringbuffer_handle handle, void *item, size_t size);

bool ringbuffer_pop(ringbuffer_handle handle, void *out, size_t size);

size_t ringbuffer_size(ringbuffer_handle handle);

size_t ringbuffer_capacity(ringbuffer_handle handle);

#ifdef __cplusplus
}
#endif

#endif