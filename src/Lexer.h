#pragma once
#include <string>

enum TokenType{
    IDENT,
    LPAREN,
    RPAREN,
    DEF,
    MAIN,
    NEWLINE,
    END_OF_FILE,
    COMMA,
    WHITESPACE,
    STRING,
    NONE
};

struct Token{

    std::string Literal;
    TokenType Type;

    Token(std::string literal, TokenType tokenType)
        : Literal(literal), Type(tokenType){};
    
    Token() 
        : Literal(""), Type(NONE){};
};

class Lexer{

    public:
       
        Lexer(std::string input);
        Token nextToken();
        Token peekToken();

    private:

        std::string readIdentifer();
        std::string readTabs();
        std::string readStringLiteral();

        bool isWhiteSpace();

        std::string readWhiteSpace();

        void skip();
        bool isSkippable();
        bool isLetter();
        void readChar();

        char ch;
        std::string input;
        int position;
        int readPosition;

};