#include "Chunk.hpp"

Chunk::Chunk()
    : mCount(0)
    , mCapacity(0)
    , mCode(nullptr)
    , mLines(nullptr)
{
}

Chunk::~Chunk()
{
    clear();
}

void Chunk::clear()
{
    MEMORY_FREE_ARRAY(uint8_t, mCode, mCapacity);
    MEMORY_FREE_ARRAY(int, mLines, mCapacity);
    mCount = 0;
    mCapacity = 0;
    mCode = nullptr;
    mLines = nullptr;
}

void Chunk::push(std::uint8_t byte, int line)
{
    if (mCapacity < mCount + 1)
    {
        reserve(MEMORY_GROW_CAPACITY(mCapacity));
    }

    mCode[mCount] = byte;
    mLines[mCount] = line;
    mCount++;
}

void Chunk::reserve(std::size_t size)
{
    if (mCapacity < size)
    {
        mCode = MEMORY_GROW_ARRAY(mCode, std::uint8_t, mCapacity, size);
        mLines = MEMORY_GROW_ARRAY(mLines, int, mCapacity, size);
        mCapacity = size;
    }
}

std::size_t Chunk::addConstant(Value value)
{
    mConstants.push(value);
    return mConstants.size() - 1;
}

std::size_t Chunk::size() const
{
    return mCount;
}

std::size_t Chunk::capacity() const
{
    return mCapacity;
}

const int& Chunk::getLine(std::size_t index) const
{
    // TODO : Throw exception ?
    return mLines[index];
}

const std::uint8_t& Chunk::getCode(std::size_t index) const
{
    // TODO : Throw exception ?
    return mCode[index];
}

const Value& Chunk::getConstant(std::size_t constantIndex) const
{
    return mConstants[constantIndex];
}

std::uint8_t* Chunk::beginOfCode()
{
    return mCode;
}

