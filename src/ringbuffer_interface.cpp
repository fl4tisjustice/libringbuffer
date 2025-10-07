#include <stdexcept>

#include <cstdio>

#include "ringbuffer.hpp"
#include "ringbuffer_interface.h"

template class RingBuffer<char>;

extern "C" {
    ringbuffer_handle ringbuffer_create(size_t minimum_count, size_t elem_size) {
        return reinterpret_cast<ringbuffer_handle>(new RingBuffer<char>(minimum_count * elem_size));
    }
    
    void ringbuffer_free(ringbuffer_handle handle) {
        delete reinterpret_cast<RingBuffer<char>*>(handle);
    }

    bool ringbuffer_enqueue(ringbuffer_handle handle, void *item, size_t size) {
        try {
            reinterpret_cast<RingBuffer<char>*>(handle)->enqueueMany(std::span<char>(static_cast<char*>(item), size));
        } catch (const std::length_error &e) {
            fprintf(stderr, e.what());
            return false;
        }

        return true;
    }

    bool ringbuffer_pop(ringbuffer_handle handle, void *out, size_t size) {
        try {
            return reinterpret_cast<RingBuffer<char>*>(handle)->popMany(static_cast<char*>(out), size);
        } catch (const std::out_of_range &e) {
            fprintf(stderr, e.what());
            return false;
        }

        return true;
    }

    size_t ringbuffer_size(ringbuffer_handle handle) {
        return reinterpret_cast<RingBuffer<char>*>(handle)->length();
    }

    size_t ringbuffer_capacity(ringbuffer_handle handle) {
        return reinterpret_cast<RingBuffer<char>*>(handle)->capacity();
    }
}