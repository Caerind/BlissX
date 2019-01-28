#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "Common.hpp"

#include <cstdlib>
#include <cstring>

// TODO : Chunks of Bytecode : Challenge 3

// TODO : MemoryAllocator
// TODO : MemoryTrace
class Memory
{
    public:
        Memory() = delete;

        static void* alloc(std::size_t size);
        static void* reallocate(void* pointer, std::size_t oldSize, std::size_t newSize);
        static void free(void* pointer);
};

#define MEMORY_GROW_CAPACITY(capacity) \
	((capacity) < 8 ? 8 : (capacity) * 2)

#define MEMORY_GROW_ARRAY(previous, type, oldCount, count) \
	(type*)Memory::reallocate(previous, sizeof(type) * (oldCount), sizeof(type) * (count))

#define MEMORY_FREE_ARRAY(type, pointer, oldCount) \
	Memory::reallocate(pointer, sizeof(type) * (oldCount), 0)

#define MEMORY_ALLOCATE(type, count) \
    (type*)Memory::reallocate(NULL, 0, sizeof(type) * (count))

#endif // MEMORY_HPP
