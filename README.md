# libringqueue

## Disclaimer
This is (mostly) a PoC/toy project (at least until I compare performances) and it should **NOT** be used in any real production environments.

## Motivation

After coming across [this section](https://en.wikipedia.org/wiki/Circular_buffer#Optimization) on Wikipidia's article on ring/circular buffers, I was intrigued and decided to investigate.

> *A circular-buffer implementation may be optimized by mapping the underlying buffer to two contiguous regions of virtual memory. (Naturally, the underlying buffer‘s length must then equal some multiple of the system’s page size.) Reading from and writing to the circular buffer may then be carried out with greater efficiency by means of direct memory access; those accesses which fall beyond the end of the first virtual-memory region will automatically wrap around to the beginning of the underlying buffer. When the read offset is advanced into the second virtual-memory region, both offsets—read and write—are decremented by the length of the underlying buffer.*

As I had little to no experience with the low-level memory management APIs (both on Windows and on Linux) despite knowing how they work on a rudimentary level, I figured it would be good practice to play around with this concept.

Additionally, with [Advent of Code](https://adventofcode.com/) coming up in a couple months (and remembering how I went about [Day 10](https://github.com/fl4tisjustice/AoC2024/blob/f56f4f9b47da6e8839204eaca22332e1eb1995ea/day-10/day-10.c#L37C1-L37C7)), it was the perfect opportunity to explore the concept. (With some luck it'll be good enough for me to use.)

However, as is often the case for me, I kept wanting to build on top of the concept. \
It started out as me testing the concept in both C and Zig and struggling to find a good solution that would allow me to write a library from Zig usable with C and failing miserably. (In hindsight, I could have done something similar to what I did here with `anyopaque*`, but it'd been months since I had last even touched Zig, it was bound to go wrong.)

So after spending some time mulling over it, I decided to start with a C++ implementation, and from that provide a C ABI-compatible interface that I could then use with whatever I needed, and that *finally* worked out.

As a disclaimer that is merely a skill issue.

## Building

As long as one has CMake and Make installed, it should be as simple as running `make` in the project root directory and the rest should be taken care of.

By default, the `Release` target will be built, but if for some reason one needs the `Debug` build, it is available by running `make debug` (likewise, `make release` will build the `Release` target, while `make all` will build both). The library will then be available in the `lib` folder.

The library is usable as-is, either with C++ (header-only library, so as to make use of the metaprogramming features) or C (dynamically linked library).

Aditionally, Programs linked against the library *must* used a C++-capable linker whether that is `gcc` with the `--stdc++` option, directly using `g++`, or any other suitable linker.

Aside from that, no other flags are necessary to link against the library, as any dependencies are statically linked in the library (C/C++ runtimes on Windows; WinAPI functions will be available through KERNEL32.DLL, which is present in every Windows host and injected indiscriminately at runtime, while on Linux, the standard libraries are "always" available system-wide).

## Usage

The library currently exposes a few functions:

```cpp
// C++
template <typename T>
RingQueue(size_t mininumCount)

// C
ringqueue_handle ringqueue_create(size_t minimum_count, size_t elem_size);
```

These are the ring buffer constructor and allocator respectively. They will provide a ring buffer object (C++) or an opaque pointer to a ring buffer object (C).
All of the initial memory setup is taken care of here (as described [here](#motivation)), and can subsequently fail.
The caller should watch out for errors and deal with them accordingly.

Or don't, I'm not your mom.

Particularly, in the case of C, there is an additional function supplied:

```c
void ringqueue_free(ringqueue_handle handle);
```

As the name implies, it will free the previously allocated ring buffer.
As it often is with C, the caller must explicitly manage the memory.
On the other hand, the C++ implementation frees all the allocated memory in its destructor when it releases the memory backing.

The main functionality of the library is currently provided by the subsequent functions: 

```c++
// C++
template <typename T>
bool enqueue(T &&item);

template <typename T>
[[ nodiscard ]] T pop();

// C
bool ringqueue_enqueue(ringqueue_handle handle, void *item, size_t size);

bool ringqueue_pop(ringqueue_handle handle, void *out, size_t size);
```
While `enqueue()` is used to push an element to the back of the queue, `pop()` is used to pop an element off the front.
(Additional `enqueueMany()` and `popMany()` are provided in the C++ implementation, but they're mostly there to facilitate the usage of the C interface, and fall outside the scope of the immediately projectable use for now.)

All of these have inherent bounds checking and can throw exceptions when either popping an element off of an empty queue or attempting to enqueue an element past the maximum size. 

Finally, the following helper functions are also available:

```c++
// C++
template <typename T>
size_t length() const;

template <typename T>
size_t capacity() const;

// C
size_t ringqueue_size(ringqueue_handle handle);
size_t ringqueue_capacity(ringqueue_handle handle);
```
## What I Learned (Am Learning?)
- Duck typing (somehow never messed around with it before)
- Compiling into Shared Object (on Linux) and Dynamically Linked Library (on Windows) and everything that entails
- Virtual memory management (anonymous files, tmpfs, memory mapping, etc.)

## Goals
- [x] Cross-platform
- [x] Switch to CMake
- [ ] Testing suite
- [x] CI/CD
- [ ] Better error handling (almost nonexistent right now)
- [ ] Support `-fno-exceptions`
- [ ] Documentation