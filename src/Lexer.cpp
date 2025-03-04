#include "Lexer.h"

Lexer::Lexer(std::string input)
{
    this->input = input;
    ch = input[0];
    readPosition = 0;
    position = 0;
}

Token Lexer::nextToken()
{
    TokenType tokenType;
    std::string literal;

    switch (ch)
    {
    case '(':
        tokenType = LPAREN;
        literal = std::string{ch};
        readChar();
        break;

    case ')':
        tokenType = RPAREN;
        literal = std::string{ch};
        readChar();
        break;

    case ',':
        tokenType = COMMA;
        literal = std::string{ch};
        readChar();
        break;
    
    case '\n':
        tokenType = NEWLINE;
        literal = std::string{ch};
        readChar();
        break;
    case 0:
        tokenType = END_OF_FILE;
        literal = "";
        readChar();
        break;
    
    default:
        if(isLetter()){
            tokenType = IDENT;
            literal = readIdentifer();

            if(literal == "def"){
                tokenType = DEF;
            }
        }
        else if(isWhiteSpace()){
            tokenType = WHITESPACE;
            literal = readWhiteSpace();
        }
        else if(ch == '"'){
            tokenType = STRING;
            literal = readStringLiteral();
        }
       
    }
    
    return Token(literal, tokenType);
}

Token Lexer::peekToken()
{
    return Token("", IDENT);
}

std::string Lexer::readIdentifer()
{
   int startPosition = this->position;
   while(isLetter()){
    readChar();
   }
   return input.substr(startPosition, this->position - startPosition);
}

std::string Lexer::readTabs()
{
    int startPosition = this->position;
    while(isLetter()){
     readChar();
    }
    return input.substr(startPosition, this->position - startPosition);
}

std::string Lexer::readStringLiteral()
{
    int startPosition = this->position;
    readChar();
    while(ch != '"'){
        readChar();
    }
    readChar();
    return input.substr(startPosition + 1, (this->position - 1) - startPosition);
}

bool Lexer::isWhiteSpace()
{
   return ch == '\t' || ch == ' ';
}

std::string Lexer::readWhiteSpace(){
    int startPosition = this->position;
    while(isWhiteSpace()){
        readChar();
    }
    return input.substr(startPosition, this->position - startPosition);
}

bool Lexer::isLetter()
{
   return 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == '_';
}

void Lexer::readChar()
{
    if(readPosition >= input.size()){
        ch = 0;
    }
    else{
        ch = input[readPosition];
    }
    position = readPosition;
    readPosition += 1;
}
