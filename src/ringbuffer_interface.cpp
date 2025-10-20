#include <stdexcept>

#include <cstdio>

#include "ringqueue.hpp"
#include "ringqueue_interface.h"

template class RingQueue<uint8_t>;

extern "C" {
    ringqueue_handle ringqueue_create(size_t minimum_count, size_t elem_size) {
        return reinterpret_cast<ringqueue_handle>(new RingQueue<uint8_t>(minimum_count * elem_size));
    }
    
    void ringqueue_free(ringqueue_handle handle) {
        delete reinterpret_cast<RingQueue<uint8_t>*>(handle);
    }

    bool ringqueue_enqueue(ringqueue_handle handle, void *item, size_t size) {
        try {
            reinterpret_cast<RingQueue<uint8_t>*>(handle)->enqueueMany(std::span<uint8_t>(static_cast<uint8_t*>(item), size));
        } catch (const std::length_error &e) {
            fprintf(stderr, "%s\n", e.what());
            return false;
        }

        return true;
    }

    bool ringqueue_pop(ringqueue_handle handle, void *out, size_t size) {
        try {
            return reinterpret_cast<RingQueue<uint8_t>*>(handle)->popMany(static_cast<uint8_t*>(out), size);
        } catch (const std::out_of_range &e) {
            fprintf(stderr, "%s\n", e.what());
            return false;
        }

        return true;
    }

    size_t ringqueue_size(ringqueue_handle handle) {
        return reinterpret_cast<RingQueue<uint8_t>*>(handle)->length();
    }

    size_t ringqueue_capacity(ringqueue_handle handle) {
        return reinterpret_cast<RingQueue<uint8_t>*>(handle)->capacity();
    }
}