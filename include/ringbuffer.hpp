#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <cstring>
#include <cassert>

#include <cstddef>
#include <cstdint>

#include <unistd.h>
#include <sys/mman.h>

#include <span>
#include <vector>
#include <stdexcept>

#define MESSAGE_PREFIX(tag) "[" #tag "]\t"

#define LENGTH_ERROR_MESSAGE \
    MESSAGE_PREFIX(ERROR)"Queue has reached maximum capacity. Cannot enqueue more.\n"

#define OUT_OF_RANGE_ERROR_MESSAGE \
    MESSAGE_PREFIX(ERROR)"Attempted to pop element off of an empty buffer.\n"

template <typename T>
class RingBuffer {
    private:
        uintptr_t ptr;
        size_t size;
        ptrdiff_t write, read = 0;

        inline ptrdiff_t toNormalized(ptrdiff_t offset) const {
            return offset % size;
        }

        enum class Intent {
            WRITE, READ
        };

        inline bool check_bounds(Intent intent, size_t count = 1) const {
            const auto span = static_cast<size_t>(write - read);

            switch (intent) {
                case Intent::WRITE: return span + count * sizeof(T) < size;
                case Intent::READ: return span >= count * sizeof(T);
                default:
                    assert(false && "Unreachable");
            }
        }

    public:
        RingBuffer(size_t mininumCount) {
            const size_t page_size = ::getpagesize();
            const size_t pages = mininumCount * sizeof(T) / page_size + 1;
            const size_t size = pages * page_size;

            int fd = memfd_create("ringbuffer", MFD_CLOEXEC);
            ftruncate(fd, size);

            void *const shm = mmap(nullptr, size * 2, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            
            mmap(shm, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
            mmap(static_cast<char*>(shm) + size, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);

            this->ptr = reinterpret_cast<uintptr_t>(shm);
            this->size = size;
        }

        bool enqueue(T &&item) {
            if (!check_bounds(Intent::WRITE)) [[ unlikely ]] throw std::length_error(LENGTH_ERROR_MESSAGE);

            *reinterpret_cast<T*>(ptr + toNormalized(write)) = item;
            write += sizeof(T);
            return true;
        }

        bool enqueueMany(std::span<T> &&items) {
            if (!check_bounds(Intent::WRITE, items.size())) [[ unlikely ]] throw std::length_error(LENGTH_ERROR_MESSAGE);
            ::memcpy(reinterpret_cast<T*>(ptr + toNormalized(write)), items.data(), items.size_bytes());
            write += items.size_bytes();
            return true;
        }

        [[ nodiscard ]] T pop() {
            if (!check_bounds(Intent::READ)) [[ unlikely ]] throw std::out_of_range(OUT_OF_RANGE_ERROR_MESSAGE);
            T ret = *reinterpret_cast<T*>(ptr + toNormalized(read));
            read += sizeof(T);
            return ret;
        }

        [[ nodiscard ]] std::vector<T> popMany(size_t count) {
            if (!check_bounds(Intent::READ, count)) [[ unlikely ]] throw std::out_of_range(OUT_OF_RANGE_ERROR_MESSAGE);
            const T *const begin = reinterpret_cast<T*>(ptr + toNormalized(read));
            const T *const end = begin + count;
            return std::vector<T>(begin, end);
        }

        [[ nodiscard ]] bool popMany(T *out, size_t count) {
            if (!check_bounds(Intent::READ, count)) [[ unlikely ]] throw std::out_of_range(OUT_OF_RANGE_ERROR_MESSAGE);
            const size_t bytes = count * sizeof(T);
            ::memcpy(out, reinterpret_cast<T*>(ptr + toNormalized(read)), bytes);
            read += bytes;
            return true;
        }

        size_t length() const {
            return static_cast<size_t>(write - read) / sizeof(T);
        }

        size_t capacity() const {
            return size / sizeof(T);
        }

        ~RingBuffer() {
            munmap(reinterpret_cast<T*>(ptr), size);
        }
};

#endif