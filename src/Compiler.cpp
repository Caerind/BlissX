#include "Compiler.hpp"

#include <cstdio>

#ifdef DEBUG_PRINT_CODE
    #include "Debug.hpp"
#endif

bool Compiler::compile(const char* source, Chunk* chunk)
{
    Scanner::getInstance().newSource(source);

    mChunk = chunk;
    mParser.hadError = false;
    mParser.panicMode = false;

    advance();

    expression();

    consume(Token::Type::Token_EndOfFile, "Expect end of expression.");

    endCompiler();

    return !mParser.hadError;
}

void Compiler::advance()
{
    mParser.previous = mParser.current;
    for (;;)
    {
        mParser.current = Scanner::getInstance().scanToken();
        if (mParser.current.type != Token::Type::Token_Error) break;
        errorAtCurrent(mParser.current.start);
    }
}

void Compiler::errorAtCurrent(const char* message)
{
    errorAt(&mParser.current, message);
}

void Compiler::errorAt(Token* token, const char* message)
{
    if (mParser.panicMode) return;
    mParser.panicMode = true;

    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == Token::Type::Token_EndOfFile)
    {
        fprintf(stderr, " at end");
    }
    else if (token->type == Token::Type::Token_Error)
    {
        // Nothing
    }
    else
    {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    mParser.hadError = true;
}

void Compiler::consume(Token::Type type, const char* message)
{
    if (mParser.current.type == type)
    {
        advance();
        return;
    }

    errorAtCurrent(message);
}

void Compiler::emitByte(std::uint8_t byte)
{
    mChunk->push(byte, mParser.previous.line);
}

void Compiler::emitBytes(std::uint8_t byte1, std::uint8_t byte2)
{
    emitByte(byte1);
    emitByte(byte2);
}

void Compiler::emitReturn()
{
    emitByte(Chunk::OpCode::Op_Return);
}

void Compiler::emitConstant(Value value)
{
    emitBytes(Chunk::OpCode::Op_Constant, makeConstant(value));
}

void Compiler::endCompiler()
{
    emitReturn();

    #ifdef DEBUG_PRINT_CODE
    if (!mParser.hadError)
    {
        Debug::disassembleChunk(*mChunk, "code");
    }
    #endif // DEBUG_PRINT_CODE
}

void Compiler::literal()
{
    switch (mParser.previous.type)
    {
        case Token::Type::Token_False: emitByte(Chunk::OpCode::Op_False); break;
        case Token::Type::Token_Null: emitByte(Chunk::OpCode::Op_Null); break;
        case Token::Type::Token_True: emitByte(Chunk::OpCode::Op_True); break;
        default:
            return; // Unreachable
    }
}

void Compiler::binary()
{
    // Remember the operator
    Token::Type operatorType = mParser.previous.type;

    // Compile the right operand
    const ParseRule* rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));

    // Emit the operator instruction
    switch (operatorType)
    {
        case Token::Type::Token_EqualEqual: emitByte(Chunk::OpCode::Op_Equal); break;
        case Token::Type::Token_BangEqual: emitByte(Chunk::OpCode::Op_BangEqual); break;
        case Token::Type::Token_Greater: emitByte(Chunk::OpCode::Op_Greater); break;
        case Token::Type::Token_GreaterEqual: emitByte(Chunk::OpCode::Op_GreaterEqual); break;
        case Token::Type::Token_Less: emitByte(Chunk::OpCode::Op_Less); break;
        case Token::Type::Token_LessEqual: emitByte(Chunk::OpCode::Op_LessEqual); break;
        case Token::Type::Token_Plus: emitByte(Chunk::OpCode::Op_Add); break;
        case Token::Type::Token_Minus: emitByte(Chunk::OpCode::Op_Substract); break;
        case Token::Type::Token_Star: emitByte(Chunk::OpCode::Op_Multiply); break;
        case Token::Type::Token_Slash: emitByte(Chunk::OpCode::Op_Divide); break;
        default:
            return; // Unreachable
    }
}

void Compiler::grouping()
{
    expression();
    consume(Token::Type::Token_RightParen, "Expect ')' after expression.");
}

void Compiler::number()
{
    double value = strtod(mParser.previous.start, nullptr);
    emitConstant(Value(value));
}

void Compiler::string()
{
    emitConstant(Value((Obj*)ObjString::copyString(mParser.previous.start + 1, mParser.previous.length - 2)));
}

void Compiler::unary()
{
    Token::Type operatorType = mParser.previous.type;

    // Compile the operand
    parsePrecedence(Compiler::Precedence::Prec_Unary);

    // Emit the operator instruction
    switch (operatorType)
    {
        case Token::Type::Token_Bang: emitByte(Chunk::OpCode::Op_Not); break;
        case Token::Type::Token_Minus: emitByte(Chunk::OpCode::Op_Negate); break;
        default:
            return; // Unreachable
    }
}

void Compiler::expression()
{
    parsePrecedence(Compiler::Precedence::Prec_Assignment);
}

void Compiler::parsePrecedence(Precedence precedence)
{
    advance();

    ParseFn prefixRule = getRule(mParser.previous.type)->prefix;
    if (prefixRule == nullptr)
    {
        errorAtCurrent("Expect expression.");
        return;
    }

    prefixRule();

    while (precedence <= getRule(mParser.current.type)->precedence)
    {
        advance();
        ParseFn infixRule = getRule(mParser.previous.type)->infix;
        if (infixRule != nullptr)
        {
            infixRule();
        }
    }
}

const Compiler::ParseRule* Compiler::getRule(Token::Type type)
{
    return &mRules[type];
}

std::uint8_t Compiler::makeConstant(Value value)
{
    int constant = mChunk->addConstant(value);
    if (constant > UINT8_MAX)
    {
        errorAtCurrent("Too many constants in one chunk.");
        return 0;
    }
    return (uint8_t)constant;
}

Chunk* Compiler::mChunk = nullptr;

Compiler::Parser Compiler::mParser;

const Compiler::ParseRule Compiler::mRules[] = {
    { Compiler::grouping,   nullptr,            Prec_Call },        // Token_LeftParen
    { nullptr,              nullptr,            Prec_None },        // Token_RightParen
    { nullptr,              nullptr,            Prec_None },        // Token_LeftBrace
    { nullptr,              nullptr,            Prec_None },        // Token_RightBrace
    { nullptr,              nullptr,            Prec_None },        // Token_Comma
    { nullptr,              nullptr,            Prec_Call },        // Token_Dot
    { Compiler::unary,      Compiler::binary,   Prec_Term },        // Token_Minus
    { nullptr,              Compiler::binary,   Prec_Term },        // Token_Plus
    { nullptr,              nullptr,            Prec_None },        // Token_Semicolon
    { nullptr,              Compiler::binary,   Prec_Factor },      // Token_Slash
    { nullptr,              Compiler::binary,   Prec_Factor },      // Token_Star
    { Compiler::unary,      nullptr,            Prec_None },        // Token_Bang
    { nullptr,              Compiler::binary,   Prec_Equality },    // Token_BangEqual
    { nullptr,              nullptr,            Prec_None },        // Token_Equal
    { nullptr,              Compiler::binary,   Prec_Equality },    // Token_EqualEqual
    { nullptr,              Compiler::binary,   Prec_Comparison },  // Token_Greater
    { nullptr,              Compiler::binary,   Prec_Comparison },  // Token_GreaterEqual
    { nullptr,              Compiler::binary,   Prec_Comparison },  // Token_Less
    { nullptr,              Compiler::binary,   Prec_Comparison },  // Token_LessEqual
    { nullptr,              nullptr,            Prec_None },        // Token_Identifier
    { Compiler::string,     nullptr,            Prec_None },        // Token_String
    { Compiler::number,     nullptr,            Prec_None },        // Token_Number
    { nullptr,              nullptr,            Prec_And },         // Token_And
    { nullptr,              nullptr,            Prec_None },        // Token_Class
    { nullptr,              nullptr,            Prec_None },        // Token_Else
    { Compiler::literal,    nullptr,            Prec_None },        // Token_False
    { nullptr,              nullptr,            Prec_None },        // Token_Func
    { nullptr,              nullptr,            Prec_None },        // Token_For
    { nullptr,              nullptr,            Prec_None },        // Token_If
    { Compiler::literal,    nullptr,            Prec_None },        // Token_Null
    { nullptr,              nullptr,            Prec_Or },          // Token_Or
    { nullptr,              nullptr,            Prec_None },        // Token_Print
    { nullptr,              nullptr,            Prec_None },        // Token_Return
    { nullptr,              nullptr,            Prec_None },        // Token_Super
    { nullptr,              nullptr,            Prec_None },        // Token_This
    { Compiler::literal,    nullptr,            Prec_None },        // Token_True
    { nullptr,              nullptr,            Prec_None },        // Token_Var
    { nullptr,              nullptr,            Prec_None },        // Token_While
    { nullptr,              nullptr,            Prec_None },        // Token_Error
    { nullptr,              nullptr,            Prec_None },        // Token_EndOfFile
};
