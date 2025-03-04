#include "Parser.h"
#include <cstring>

Parser::Parser(const std::string input)
 : lexer(Lexer(input))
{
    nextToken();
    nextToken();
}

std::unique_ptr<ExprNode> Parser::parseExpression()
{
   std::unique_ptr<ExprNode> expr = nullptr;
   if(curToken.Type == STRING)
   {
     // I think a better design for this would be some sort of a constants table?
     char* copiedString = new char[curToken.Literal.length() + 1];
     std::strcpy(copiedString, curToken.Literal.c_str());
     expr = std::make_unique<LiteralExprNode>("string", copiedString);
   }
   
   nextToken();
   return expr;
}

std::vector<std::unique_ptr<ExprNode>> Parser::parseCallArguments()
{
    std::vector<std::unique_ptr<ExprNode>> arguments;
    if(curToken.Type == RPAREN)
    {
        return arguments;
    }

    arguments.push_back(parseExpression());

    while(peekToken.Type == COMMA)
    {
        nextToken();
        nextToken();
        arguments.push_back(parseExpression());
    }
    
    return arguments;
}

std::unique_ptr<CallExprNode> Parser::parseCallExpr()
{
    if(curToken.Type == IDENT && peekToken.Type == LPAREN)
    {
        std::string functionName = curToken.Literal;

        // skip two token
        nextToken();
        nextToken();
        auto args = parseCallArguments();
        return std::make_unique<CallExprNode>(functionName, std::move(args));
    }
    else
    {
        // just return a null ptr to avoid error handling
        // for now.
        return nullptr;
    }
   
}

std::unique_ptr<FunctionNode> Parser::parseFunctionDefinition()
{
    std::vector<std::unique_ptr<StatementNode>> body;
    std::unique_ptr<PrototypeNode> protoType;

    nextToken();
    while(curToken.Type != IDENT)
    {
        nextToken();
    }

    protoType = std::make_unique<PrototypeNode>(curToken.Literal);
    nextToken();

    if(curToken.Type == LPAREN)
    {
        nextToken();
    }

    while(curToken.Type != RPAREN)
    {
       nextToken();
    }

    nextToken();
    while(curToken.Type != NEWLINE)
    {
        nextToken();
    }
    nextToken();

    std::string firstWhiteSpaceDelimit;
    if(curToken.Type == WHITESPACE)
    {
        firstWhiteSpaceDelimit = curToken.Literal;
    }

    while(curToken.Literal == firstWhiteSpaceDelimit)
    {
        nextToken();
        auto callExpr = parseCallExpr();
        auto statement = std::make_unique<ExprStatementNode>(std::move(callExpr));
        body.push_back(std::move(statement));
        nextToken();
    }
    return std::make_unique<FunctionNode>(std::move(protoType), std::move(body));
}

std::unique_ptr<ProgramNode> Parser::parseProgram()
{
    auto programNode = std::make_unique<ProgramNode>();
    while (curToken.Type != END_OF_FILE)
    {
        std::unique_ptr<StatementNode> statement = nullptr;
        if(curToken.Type == DEF)
        {
           statement = parseFunctionDefinition();
        }

        if(statement != nullptr)
        {
            programNode.get()->addStatement(std::move(statement));
        }
    }
    
    return programNode;
}

void Parser::nextToken()
{
    curToken = peekToken;
    peekToken = lexer.nextToken();
}
