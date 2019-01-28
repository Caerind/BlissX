#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "Common.hpp"

// TODO : Scanning on Demand : Challenge 1
// TODO : Scanning on Demand : Challenge 2
// TODO : Scanning on Demand : Challenge 3

struct Token
{
    enum Type
    {
        // Single-character tokens.
        Token_LeftParen, Token_RightParen,
        Token_LeftBrace, Token_RightBrace,
        Token_Comma, Token_Dot, Token_Minus, Token_Plus,
        Token_Semicolon, Token_Slash, Token_Star,

        // One or two character tokens.
        Token_Bang, Token_BangEqual,
        Token_Equal, Token_EqualEqual,
        Token_Greater, Token_GreaterEqual,
        Token_Less, Token_LessEqual,

        // Literals.
        Token_Identifier, Token_String, Token_Number,

        // Keywords.
        Token_And, Token_Class, Token_Else, Token_False,
        Token_Func, Token_For, Token_If, Token_Null, Token_Or,
        Token_Print, Token_Return, Token_Super, Token_This,
        Token_True, Token_Var, Token_While,

        Token_Error,
        Token_EndOfFile
    };

    Token();
    Token(Type pType, const char* pStart, int pLength, int pLine);

    Type type;
    const char* start;
    int length;
    int line;
};

class Scanner
{
    public:
        static Scanner& getInstance();

        void newSource(const char* source);

        Token scanToken();

    private:
        Scanner();
        ~Scanner();

        Token string();
        Token number();
        Token identifier();

        Token makeToken(Token::Type type) const;
        Token errorToken(const char* message) const;

        Token::Type identifierType() const;
        Token::Type checkKeyword(int start, int length, const char* rest, Token::Type type) const;

        bool isAtEnd() const;
        char advance();
        bool match(char expected);
        void skipWhitespace();
        char peek() const;
        char peekNext() const;
        bool isDigit(char c) const;
        bool isAlpha(char c) const;

    private:
        const char* mStart;
        const char* mCurrent;
        int mLine;
};

#endif // SCANNER_HPP
