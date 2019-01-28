#ifndef DEBUG_HPP
#define DEBUG_HPP

#include "Chunk.hpp"

#include <cstdio>

class Debug
{
    public:
        Debug() = delete;

        static void disassembleChunk(const Chunk& chunk, const char* name);

        static std::size_t disassembleInstruction(const Chunk& chunk, std::size_t offset);

        static void printValue(Value value);
        static void printObject(Value value);

    private:
        static std::size_t constantInstruction(const char* name, const Chunk& chunk, std::size_t offset);
        static std::size_t simpleInstruction(const char* name, std::size_t offset);
};

#endif // DEBUG_HPP
