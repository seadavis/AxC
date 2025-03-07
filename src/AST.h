#pragma once

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <memory>
#include <vector>

class ProgramNode;
class ExprStatementNode;
class LiteralExprNode;
class CallExprNode;
class FunctionNode;

using namespace llvm;

class TreeWalker{

    public:
        TreeWalker(ProgramNode* node);

        // here ownership of module transfers to the caller
        // I do actually think this is correct
        Module* generateIR();

        virtual Value* visit(ExprStatementNode* statementNode);
        virtual Value* visit(LiteralExprNode* exprNode);
        virtual Value* visit(CallExprNode* exprNode);
        virtual Value* visit(FunctionNode* functionNode);

    private:
        ProgramNode* astRoot;
        std::unique_ptr<LLVMContext> context;
        std::unique_ptr<Module> module;
        std::unique_ptr<IRBuilder<>> builder;

};

class Node{
    public:
        virtual Value* Accept(TreeWalker* walker) = 0;
};

class StatementNode : public Node{
    public:
        virtual ~StatementNode() = default;
};

class ExprNode : public Node {
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

        Value*  Accept(TreeWalker* walker) override{
            return walker->visit(this);
        }

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

        Value* Accept(TreeWalker* walker) override{
          return walker->visit(this);
        }
        std::unique_ptr<ExprNode> Expr;
};

class CallExprNode : public ExprNode{

    public:
        CallExprNode(std::string functionName, 
                    std::vector<std::unique_ptr<ExprNode>> arguments) :
                FunctionName(functionName), Arguments(std::move(arguments)) {};

        Value* Accept(TreeWalker* walker) override{
           return walker->visit(this);
        }

        std::string FunctionName;
        std::vector<std::unique_ptr<ExprNode>> Arguments;
};

class ProgramNode {


    public:
        std::vector<std::unique_ptr<StatementNode>> Statements;
        virtual ~ProgramNode() = default;
};

class PrototypeNode {

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

    Value* Accept(TreeWalker* walker) override{
        return walker->visit(this);
    }
};