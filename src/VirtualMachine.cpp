#include "VirtualMachine.hpp"

VirtualMachine::VirtualMachine()
{
    resetStack();
}

VirtualMachine::~VirtualMachine()
{
}

void VirtualMachine::push(Value value)
{
    *mStackTop = value;
    mStackTop++;
}

Value VirtualMachine::pop()
{
    mStackTop--;
    return *mStackTop;
}

void VirtualMachine::resetStack()
{
    mStackTop = mStack;
}

void VirtualMachine::runtimeError(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    std::size_t instruction = mInstructionPointer - mChunk->beginOfCode();
    fprintf(stderr, "[line %d] in script\n", mChunk->getLine(instruction));

    resetStack();
}

VirtualMachine::InterpretResult VirtualMachine::interpret(const char* source)
{
    Chunk chunk;

    if (!Compiler::compile(source, &chunk))
    {
        return Interpret_CompileError;
    }

    mChunk = &chunk;
    mInstructionPointer = mChunk->beginOfCode();

    return run();
}

VirtualMachine::InterpretResult VirtualMachine::run()
{
    #define READ_BYTE() (*mInstructionPointer++)
    #define READ_CONSTANT() (mChunk->getConstant(READ_BYTE()))
    #define BINARY_OP(op) \
        do { \
            if (!peek(0).isNumber() || !peek(1).isNumber()) \
            { \
                runtimeError("Operands must be numbers."); \
                return Interpret_RuntimeError; \
            } \
            double b = pop().asNumber(); \
            double a = pop().asNumber(); \
            push(Value(a op b)); \
        } while(false)

    for(;;)
    {
        #ifdef DEBUG_TRACE_EXECUTION
            printf("          ");
            for (Value* slot = mStack; slot < mStackTop; slot++)
            {
                printf("[ ");
                Debug::printValue(*slot);
                printf(" ]");
            }
            printf("\n");
            Debug::disassembleInstruction(*mChunk, (int)(mInstructionPointer - mChunk->beginOfCode()));
        #endif

        uint8_t instruction;
        switch (instruction = READ_BYTE())
        {
            case Chunk::Op_Constant: push(READ_CONSTANT()); break;
            case Chunk::Op_Null: push(Value()); break;
            case Chunk::Op_True: push(Value(true)); break;
            case Chunk::Op_False: push(Value(false)); break;
            case Chunk::Op_Equal: push(Value(pop().isEquals(pop()))); break;
            case Chunk::Op_BangEqual: push(Value(!pop().isEquals(pop()))); break;
            case Chunk::Op_Greater: BINARY_OP(>); break;
            case Chunk::Op_GreaterEqual: BINARY_OP(>=); break;
            case Chunk::Op_Less: BINARY_OP(<); break;
            case Chunk::Op_LessEqual: BINARY_OP(<=); break;
            case Chunk::Op_Add:
            {
                if (peek(0).isString() && peek(1).isString())
                {
                    concatenate();
                }
                else if (peek(0).isNumber() && peek(1).isNumber())
                {
                    push(Value(pop().asNumber() + pop().asNumber()));
                }
                else
                {
                    runtimeError("Operands must be two numbers or two strings.");
                    return Interpret_RuntimeError;
                }
                break;
            }
            case Chunk::Op_Substract: BINARY_OP(-); break;
            case Chunk::Op_Multiply: BINARY_OP(*); break;
            case Chunk::Op_Divide: BINARY_OP(/); break;
            case Chunk::Op_Not: push(Value(pop().isFalsey())); break;
            case Chunk::Op_Negate:
                if (!peek(0).isNumber())
                {
                    runtimeError("Operand must be a number.");
                    return Interpret_RuntimeError;
                }
                push(Value(-pop().asNumber()));
                break;
            case Chunk::Op_Return:
            {
                Debug::printValue(pop());
                printf("\n");
                return Interpret_Ok;
            }
        }
    }

    #undef BINARY_OP
    #undef READ_CONSTANT
    #undef READ_BYTE
}

void VirtualMachine::concatenate()
{
    ObjString* b = pop().asString();
    ObjString* a = pop().asString();

    int length = a->length + b->length;
    char* chars = MEMORY_ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    push(Value((Obj*)ObjString::takeString(chars, length)));
}

Value VirtualMachine::peek(int distance)
{
    return mStackTop[-1 - distance];
}
