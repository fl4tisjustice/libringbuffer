#include <stdexcept>

#include <cstdio>

#include "ringqueue.hpp"
#include "ringqueue_interface.h"

template class RingQueue<char>;

extern "C" {
    ringqueue_handle ringqueue_create(size_t minimum_count, size_t elem_size) {
        return reinterpret_cast<ringqueue_handle>(new RingQueue<char>(minimum_count * elem_size));
    }
    
    void ringqueue_free(ringqueue_handle handle) {
        delete reinterpret_cast<RingQueue<char>*>(handle);
    }

    bool ringqueue_enqueue(ringqueue_handle handle, void *item, size_t size) {
        try {
            reinterpret_cast<RingQueue<char>*>(handle)->enqueueMany(std::span<char>(static_cast<char*>(item), size));
        } catch (const std::length_error &e) {
            fprintf(stderr, e.what());
            return false;
        }

        return true;
    }

    bool ringqueue_pop(ringqueue_handle handle, void *out, size_t size) {
        try {
            return reinterpret_cast<RingQueue<char>*>(handle)->popMany(static_cast<char*>(out), size);
        } catch (const std::out_of_range &e) {
            fprintf(stderr, e.what());
            return false;
        }

        return true;
    }

    size_t ringqueue_size(ringqueue_handle handle) {
        return reinterpret_cast<RingQueue<char>*>(handle)->length();
    }

    size_t ringqueue_capacity(ringqueue_handle handle) {
        return reinterpret_cast<RingQueue<char>*>(handle)->capacity();
    }
}