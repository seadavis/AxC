#pragma once

#include <string>
#include <memory>
#include <vector>

class StatementNode{
    public:
        virtual ~StatementNode() = default;
};

class ExprNode {
    public:
      virtual ~ExprNode() = default;
};

class LiteralExprNode : public ExprNode{

    public:
        LiteralExprNode(const std::string typeName, char* data);

        // I made this up to be more specific to my compiler
        // we should see how this evolves. Just did 
        // something quick and dirty so I could move on
        std::string TypeName;
        char* data;

        ~LiteralExprNode()
        {
            delete data;
        }

};

// This seems a little strange
// but it exists to hold single statements.
// Example: Function()
class ExprStatementNode : public StatementNode{
    public:
        ExprStatementNode(std::unique_ptr<ExprNode> expr) :
            Expr(std::move(expr)){};

        std::unique_ptr<ExprNode> Expr;
};

class CallExprNode : public ExprNode{

    public:
        CallExprNode(std::string functionName, 
                    std::vector<std::unique_ptr<ExprNode>> arguments) :
                FunctionName(functionName), Arguments(std::move(arguments)) {};

        std::string FunctionName;
        std::vector<std::unique_ptr<ExprNode>> Arguments;
};

class ProgramNode{


    public:
        std::vector<std::unique_ptr<StatementNode>> Statements;
        void addStatement(std::unique_ptr<StatementNode> statement);
        virtual ~ProgramNode() = default;

};

class PrototypeNode{

    public:
        std::string Name;
        PrototypeNode(const std::string &Name)
            : Name(Name) {}
};

class FunctionNode : public StatementNode {
   
  
  public:
    std::unique_ptr<PrototypeNode> Proto;
    std::vector<std::unique_ptr<StatementNode>> Body;

    FunctionNode(std::unique_ptr<PrototypeNode> Proto,
                std::vector<std::unique_ptr<StatementNode>> Body)
        : Proto(std::move(Proto)), Body(std::move(Body)) {}
};