#ifndef VIRTUALMACHINE_HPP
#define VIRTUALMACHINE_HPP

#include "Chunk.hpp"
#include "Compiler.hpp"

#include <cstdarg>

#ifdef DEBUG_TRACE_EXECUTION
    #include "Debug.hpp"
#endif

#define STACK_MAX 256

// TODO : A Virtual Machine : Challenge 1
// TODO : A Virtual Machine : Challenge 2
// TODO : A Virtual Machine : Challenge 3

// TODO : Stack size defined as instantiation
// TODO : Stack error strategy defined as instantiation

class VirtualMachine
{
    public:
        enum InterpretResult
        {
            Interpret_Ok,
            Interpret_CompileError,
            Interpret_RuntimeError
        };

        VirtualMachine();
        ~VirtualMachine();

        void push(Value value);
        Value pop();
        void resetStack();

        void runtimeError(const char* format, ...);

        InterpretResult interpret(const char* source);

    private:
        InterpretResult run();

        void concatenate();

        Value peek(int distance);

        Chunk* mChunk;
        std::uint8_t* mInstructionPointer;
        Value mStack[STACK_MAX];
        Value* mStackTop;
};

#endif // VIRTUALMACHINE_HPP
