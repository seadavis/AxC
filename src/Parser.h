#pragma once
#include "AST.h"
#include "Lexer.h"

class Parser{

    public:
        Parser(const std::string input);
        std::unique_ptr<ExprNode> parseExpression();
        std::unique_ptr<ProgramNode> parseProgram();

    private:
        Lexer lexer;
        std::vector<std::unique_ptr<ExprNode>> parseCallArguments();
        std::unique_ptr<CallExprNode> parseCallExpr();
        std::unique_ptr<FunctionNode> parseFunctionDefinition();

        void nextToken();
        Token curToken;
        Token peekToken;

};