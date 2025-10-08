#ifndef __RINGQUEUE_INTERFACE__
#define __RINGQUEUE_INTERFACE__

#include <unistd.h>

#if defined(__linux) && defined(LIBRINGQUEUE_BUILD)
    #define LIBRINGQUEUE_EXPORT __attribute__ ((visibility ("default")))
#elifdef _WIN32
    #ifdef LIBRINGQUEUE_BUILD
        #define LIBRINGQUEUE_EXPORT __declspec(dllexport)
    #else
        #define LIBRINGQUEUE_EXPORT __declspec(dllimport)
    #endif
#else
    #define LIBRINGQUEUE_EXPORT
#endif

#define ENQUEUE(handle, ptr)                                \
    ringqueue_enqueue(handle, ptr, sizeof(*ptr))

#define POP(handle, ptr)                                    \
    ringqueue_pop(handle, ptr, sizeof(*ptr))

#define LENGTH(handle, type)                                \
    (ringqueue_size(handle) / sizeof(type))

#define CAPACITY(handle, type)                              \
    (ringqueue_capacity(handle) / sizeof(type))
    
#ifdef __cplusplus
extern "C" {
#endif

struct ringqueue;

typedef struct ringqueue *ringqueue_handle;

LIBRINGQUEUE_EXPORT ringqueue_handle ringqueue_create(size_t minimum_count, size_t elem_size);

LIBRINGQUEUE_EXPORT void ringqueue_free(ringqueue_handle handle);

LIBRINGQUEUE_EXPORT bool ringqueue_enqueue(ringqueue_handle handle, void *item, size_t size);

LIBRINGQUEUE_EXPORT bool ringqueue_pop(ringqueue_handle handle, void *out, size_t size);

LIBRINGQUEUE_EXPORT size_t ringqueue_size(ringqueue_handle handle);

LIBRINGQUEUE_EXPORT size_t ringqueue_capacity(ringqueue_handle handle);

#ifdef __cplusplus
}
#endif

#endif