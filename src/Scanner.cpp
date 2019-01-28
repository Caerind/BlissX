#include "Scanner.hpp"

#include <utility>
#include <cstring>

Token::Token()
{
}

Token::Token(Token::Type pType, const char* pStart, int pLength, int pLine)
    : type(pType)
    , start(pStart)
    , length(pLength)
    , line(pLine)
{
}

Scanner& Scanner::getInstance()
{
    static Scanner instance;
    return instance;
}

void Scanner::newSource(const char* source)
{
    mStart = source;
    mCurrent = source;
    mLine = 1;
}

Token Scanner::scanToken()
{
    skipWhitespace();

    mStart = mCurrent;

    if (isAtEnd()) return std::move(makeToken(Token::Type::Token_EndOfFile));

    char c = advance();
    if (isAlpha(c)) return std::move(identifier());
    if (isDigit(c)) return std::move(number());

    switch (c)
    {
        case '(': return std::move(makeToken(Token::Type::Token_LeftParen));
        case ')': return std::move(makeToken(Token::Type::Token_RightParen));
        case '{': return std::move(makeToken(Token::Type::Token_LeftBrace));
        case '}': return std::move(makeToken(Token::Type::Token_RightBrace));
        case ';': return std::move(makeToken(Token::Type::Token_Semicolon));
        case ',': return std::move(makeToken(Token::Type::Token_Comma));
        case '.': return std::move(makeToken(Token::Type::Token_Dot));
        case '-': return std::move(makeToken(Token::Type::Token_Minus));
        case '+': return std::move(makeToken(Token::Type::Token_Plus));
        case '/': return std::move(makeToken(Token::Type::Token_Slash));
        case '*': return std::move(makeToken(Token::Type::Token_Star));
        case '!': return std::move(makeToken(match('=') ? Token::Type::Token_BangEqual : Token::Type::Token_Bang));
        case '=': return std::move(makeToken(match('=') ? Token::Type::Token_EqualEqual : Token::Type::Token_Equal));
        case '<': return std::move(makeToken(match('=') ? Token::Type::Token_LessEqual : Token::Type::Token_Less));
        case '>': return std::move(makeToken(match('=') ? Token::Type::Token_GreaterEqual : Token::Type::Token_Greater));
        case '"': return std::move(string());
    }

    return std::move(errorToken("Unexcepted character."));
}

Scanner::Scanner()
    : mStart(nullptr)
    , mCurrent(nullptr)
    , mLine(0)
{
}

Scanner::~Scanner()
{
}

Token Scanner::string()
{
    while (peek() != '"' && !isAtEnd())
    {
        if (peek() == '\n') mLine++;
        advance();
    }

    if (isAtEnd()) return std::move(errorToken("Unterminated string."));

    // The closing ".
    advance();
    return std::move(makeToken(Token::Type::Token_String));
}

Token Scanner::number()
{
    while (isDigit(peek())) advance();

    // Look for a fractional part
    if (peek() == '.' && isDigit(peekNext()))
    {
        // Consume the "."
        advance();
        while (isDigit(peek())) advance();
    }

    return std::move(makeToken(Token::Type::Token_Number));
}

Token Scanner::identifier()
{
    while (isAlpha(peek()) || isDigit(peek())) advance();

    return std::move(makeToken(identifierType()));
}

Token Scanner::makeToken(Token::Type type) const
{
    return std::move(Token(type, mStart, (int)(mCurrent - mStart), mLine));
}

Token Scanner::errorToken(const char* message) const
{
    return std::move(Token(Token::Type::Token_Error, message, (int)strlen(message), mLine));
}

Token::Type Scanner::identifierType() const
{
    switch (*mStart)
    {
        case 'a': return checkKeyword(1, 2, "nd", Token::Type::Token_And);
        case 'c': return checkKeyword(1, 4, "lass", Token::Type::Token_Class);
        case 'e': return checkKeyword(1, 3, "lse", Token::Type::Token_Else);
        case 'f':
            if (mCurrent - mStart > 1)
            {
                switch (mStart[1])
                {
                    case 'a': return checkKeyword(2, 3, "lse", Token::Type::Token_False);
                    case 'o': return checkKeyword(2, 1, "r", Token::Type::Token_For);
                    case 'u': return checkKeyword(2, 2, "nc", Token::Type::Token_Func);
                }
            }
            break;
        case 'i': return checkKeyword(1, 1, "f", Token::Type::Token_If);
        case 'n': return checkKeyword(1, 3, "ull", Token::Type::Token_Null);
        case 'o': return checkKeyword(1, 1, "r", Token::Type::Token_Or);
        case 'p': return checkKeyword(1, 4, "rint", Token::Type::Token_Print);
        case 'r': return checkKeyword(1, 5, "eturn", Token::Type::Token_Return);
        case 's': return checkKeyword(1, 4, "uper", Token::Type::Token_Super);
        case 't':
            if (mCurrent - mStart > 1)
            {
                switch (mStart[1])
                {
                    case 'h': return checkKeyword(2, 2, "is", Token::Type::Token_This);
                    case 'r': return checkKeyword(2, 2, "ue", Token::Type::Token_True);
                }
            }
            break;
        case 'v': return checkKeyword(1, 2, "ar", Token::Type::Token_Var);
        case 'w': return checkKeyword(1, 4, "hile", Token::Type::Token_While);

    }

    return Token::Type::Token_Identifier;
}

Token::Type Scanner::checkKeyword(int start, int length, const char* rest, Token::Type type) const
{
    if (mCurrent - mStart == start + length && memcmp(mStart + start, rest, length) == 0)
    {
        return type;
    }
    return Token::Type::Token_Identifier;
}

bool Scanner::isAtEnd() const
{
    return *mCurrent == '\0';
}

char Scanner::advance()
{
    mCurrent++;
    return mCurrent[-1];
}

bool Scanner::match(char expected)
{
    if (isAtEnd()) return false;
    if (*mCurrent != expected) return false;
    mCurrent++;
    return true;
}

void Scanner::skipWhitespace()
{
    for(;;)
    {
        char c = peek();
        switch (c)
        {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                mLine++;
                advance();
                break;
            case '/':
                if (peekNext() == '/')
                {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd()) advance();
                }
                else
                {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

char Scanner::peek() const
{
    return *mCurrent;
}

char Scanner::peekNext() const
{
    if (isAtEnd()) return '\0';
    return mCurrent[1];
}

bool Scanner::isDigit(char c) const
{
    return c >= '0' && c <= '9';
}

bool Scanner::isAlpha(char c) const
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
