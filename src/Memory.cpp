#include "Memory.hpp"

void* Memory::alloc(std::size_t size)
{
    return ::malloc(size);
}

void* Memory::reallocate(void* pointer, std::size_t oldSize, std::size_t newSize)
{
    if (newSize == 0)
    {
        Memory::free(pointer);
        return nullptr;
    }

    return ::realloc(pointer, newSize);
}

void Memory::free(void* pointer)
{
    ::free(pointer);
}
