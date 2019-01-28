#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "Chunk.hpp"
#include "Scanner.hpp"

class Compiler
{
    public:
        enum Precedence
        {
            Prec_None,
            Prec_Assignment,    // =
            Prec_Or,            // or
            Prec_And,           // and
            Prec_Equality,      // == !=
            Prec_Comparison,    // < > <= >=
            Prec_Term,          // + -
            Prec_Factor,        // * /
            Prec_Unary,         // ! - +
            Prec_Call,          // . () []
            Prec_Primary
        };

        struct Parser
        {
            Token current;
            Token previous;
            bool hadError;
            bool panicMode;
        };

        typedef void (*ParseFn)();

        struct ParseRule
        {
            ParseFn prefix;
            ParseFn infix;
            Precedence precedence;
        };

        Compiler() = delete;

        static bool compile(const char* source, Chunk* chunk);

    private:
        static void advance();
        static void errorAtCurrent(const char* message);
        static void errorAt(Token* token, const char* message);
        static void consume(Token::Type type, const char* message);

        static void emitByte(std::uint8_t byte);
        static void emitBytes(std::uint8_t byte1, std::uint8_t byte2);
        static void emitReturn();
        static void emitConstant(Value value);
        static void endCompiler();

        static void literal();
        static void binary();
        static void grouping();
        static void number();
        static void string();
        static void unary();
        static void expression();
        static void parsePrecedence(Precedence precedence);

        static const ParseRule* getRule(Token::Type type);
        static Chunk* currentChunk();
        static std::uint8_t makeConstant(Value value);

    private:
        static Chunk* mChunk;
        static Parser mParser;
        static const ParseRule mRules[];
};

#endif // COMPILER_HPP
