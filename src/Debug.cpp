#include "Debug.hpp"

void Debug::disassembleChunk(const Chunk& chunk, const char* name)
{
	printf("== %s ==\n", name);
	for (std::size_t i = 0; i < chunk.size();)
	{
		i = disassembleInstruction(chunk, i);
	}
}

std::size_t Debug::disassembleInstruction(const Chunk& chunk, std::size_t offset)
{
    printf("%04d ", offset);

    int line = chunk.getLine(offset);
	if (offset > 0 && line == chunk.getLine(offset - 1))
	{
		printf("   | ");
	}
	else
	{
		printf("%4d ", line);
	}

	uint8_t instruction = chunk.getCode(offset);
	switch (instruction)
	{
		case Chunk::Op_Constant: return constantInstruction("Op_Constant", chunk, offset);
		case Chunk::Op_Null: return simpleInstruction("Op_Null", offset);
		case Chunk::Op_True: return simpleInstruction("Op_True", offset);
		case Chunk::Op_False: return simpleInstruction("Op_False", offset);
		case Chunk::Op_Equal: return simpleInstruction("Op_Equal", offset);
		case Chunk::Op_BangEqual: return simpleInstruction("Op_BangEqual", offset);
		case Chunk::Op_Greater: return simpleInstruction("Op_Greater", offset);
		case Chunk::Op_GreaterEqual: return simpleInstruction("Op_GreaterEqual", offset);
		case Chunk::Op_Less: return simpleInstruction("Op_Less", offset);
		case Chunk::Op_LessEqual: return simpleInstruction("Op_LessEqual", offset);
		case Chunk::Op_Add: return simpleInstruction("Op_Add", offset);
		case Chunk::Op_Substract: return simpleInstruction("Op_Substract", offset);
		case Chunk::Op_Multiply: return simpleInstruction("Op_Multiply", offset);
		case Chunk::Op_Divide: return simpleInstruction("Op_Divide", offset);
		case Chunk::Op_Not: return simpleInstruction("Op_Not", offset);
		case Chunk::Op_Negate: return simpleInstruction("Op_Negate", offset);
		case Chunk::Op_Return: return simpleInstruction("Op_Return", offset);
		default: printf("Unknown opcode %d\n", instruction); return offset + 1;
	}
}

void Debug::printValue(Value value)
{
    switch (value.getType())
    {
        case Value::Type::Bool: printf(value.asBool() ? "true" : "false"); break;
        case Value::Type::Null: printf("null"); break;
        case Value::Type::Number: printf("%g", value.asNumber()); break;
        case Value::Type::Object: printObject(value); break;
    }
}

void Debug::printObject(Value value)
{
    switch (value.getObjectType())
    {
        case Obj::Type::String: printf("%s", value.asCString()); break;
    }
}

std::size_t Debug::constantInstruction(const char* name, const Chunk& chunk, std::size_t offset)
{
    std::uint8_t constant = chunk.getCode(offset + 1);
	printf("%-16s %4d '", name, constant);
	printValue(chunk.getConstant(constant));
	printf("'\n");
	return offset + 2;
}

std::size_t Debug::simpleInstruction(const char* name, std::size_t offset)
{
    printf("%s\n", name);
    return offset + 1;
}
