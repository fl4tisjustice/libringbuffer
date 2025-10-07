#ifndef __RINGBUFFER_INTERFACE__
#define __RINGBUFFER_INTERFACE__

#include <unistd.h>

#if defined(__linux) && defined(LIBRINGBUFFER_BUILD)
    #define LIBRINGBUFFER_EXPORT __attribute__ ((visibility ("default")))
#elifdef _WIN32
    #ifdef LIBRINGBUFFER_BUILD
        #define LIBRINGBUFFER_EXPORT __declspec(dllexport)
    #else
        #define LIBRINGBUFFER_EXPORT __declspec(dllimport)
    #endif
#else
    #define LIBRINGBUFFER_EXPORT
#endif

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

LIBRINGBUFFER_EXPORT ringbuffer_handle ringbuffer_create(size_t minimum_count, size_t elem_size);

LIBRINGBUFFER_EXPORT void ringbuffer_free(ringbuffer_handle handle);

LIBRINGBUFFER_EXPORT bool ringbuffer_enqueue(ringbuffer_handle handle, void *item, size_t size);

LIBRINGBUFFER_EXPORT bool ringbuffer_pop(ringbuffer_handle handle, void *out, size_t size);

LIBRINGBUFFER_EXPORT size_t ringbuffer_size(ringbuffer_handle handle);

LIBRINGBUFFER_EXPORT size_t ringbuffer_capacity(ringbuffer_handle handle);

#ifdef __cplusplus
}
#endif

#endif