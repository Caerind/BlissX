#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Value.hpp"

// TODO : Chunks of Bytecode : Challenge 1
// TODO : Chunks of Bytecode : Challenge 2

class Chunk
{
    public:
        enum OpCode
        {
            Op_Constant,
            Op_Null,
            Op_True,
            Op_False,
            Op_Equal,
            Op_BangEqual,
            Op_Greater,
            Op_GreaterEqual,
            Op_Less,
            Op_LessEqual,
            Op_Add,
            Op_Substract,
            Op_Multiply,
            Op_Divide,
            Op_Not,
            Op_Negate,
            Op_Return,
        };

        Chunk();
        ~Chunk();

        void clear();

        void push(std::uint8_t byte, int line);
        void reserve(std::size_t size);

        std::size_t addConstant(Value value);

        std::size_t size() const;
        std::size_t capacity() const;

        const int& getLine(std::size_t index) const;
        const std::uint8_t& getCode(std::size_t index) const;
        const Value& getConstant(std::size_t constantIndex) const;

        std::uint8_t* beginOfCode();

    private:
        std::size_t mCount;
        std::size_t mCapacity;
        std::uint8_t* mCode;
        int* mLines;
        ValueArray mConstants;
};

#endif // CHUNK_HPP
