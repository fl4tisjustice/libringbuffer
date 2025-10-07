#if defined(__linux) || defined(_WIN32)

#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <cstring>
#include <cassert>

#include <cstddef>
#include <cstdint>
#include <climits>

#ifdef __linux

#include <unistd.h>
#include <sys/mman.h>

#elifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif

#include <span>
#include <vector>
#include <stdexcept>

#define ANONYMOUS_FILENAME "ringbuffer"

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
        ptrdiff_t write;
        ptrdiff_t read;

        #ifdef _WIN32
        HANDLE fHandle;
        #endif

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
        RingBuffer(size_t mininumCount) : ptr(0zu), size(0zu), write(0zu), read(0zu)  {
            size_t page_size = 0;

            #ifdef __linux

            page_size = ::getpagesize();
            
            #elifdef _WIN32
            
            SYSTEM_INFO systemInfo;
            GetSystemInfo(&systemInfo);
            page_size = static_cast<size_t>(systemInfo.dwAllocationGranularity);
            
            #endif

            assert(page_size != 0 && "Page size not set");
            
            const size_t pages = mininumCount * sizeof(T) / page_size + 1;
            const size_t size = pages * page_size;

            #ifdef __linux

            int fd = memfd_create(ANONYMOUS_FILENAME, MFD_CLOEXEC);
            ftruncate(fd, size);

            void *const shm = mmap(nullptr, size * 2, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            
            mmap(shm, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
            mmap(static_cast<char*>(shm) + size, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);

            #elifdef _WIN32

            constexpr size_t DWORD_WIDTH = sizeof(DWORD) * CHAR_BIT;
            constexpr size_t MASK = (1llu << DWORD_WIDTH) - 1;

            fHandle = CreateFileMappingA(
                INVALID_HANDLE_VALUE,
                NULL,
                PAGE_READWRITE,
                static_cast<DWORD>((size >> DWORD_WIDTH) & MASK),
                static_cast<DWORD>(size & MASK),
                ANONYMOUS_FILENAME
            );

            LPVOID const shm = VirtualAlloc(
                NULL,
                static_cast<SIZE_T>(size * 2),
                MEM_RESERVE,
                PAGE_READWRITE
            );

            VirtualFree(shm, 0, MEM_RELEASE);
            
            MapViewOfFileEx(fHandle, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0, shm);
            MapViewOfFileEx(fHandle, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0, static_cast<PCHAR>(shm) + size);
            
            #endif

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
            #ifdef __linux
            munmap(reinterpret_cast<T*>(ptr), size);
            #elifdef _WIN32
            UnmapViewOfFile(reinterpret_cast<LPCVOID>(ptr));
            UnmapViewOfFile(reinterpret_cast<LPCVOID>(ptr + size));
            CloseHandle(fHandle);
            #endif
        }
};

#endif

#else
#error "Unsupported target."
#endif