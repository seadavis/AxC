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
#include "AST.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using namespace llvm;

class TreeWalker{

    public:
        TreeWalker(std::unique_ptr<ProgramNode> ast);

        // here ownership of module transfers to the caller
        // I do actually think this is correct
        Module* generateIR();

    private:
        virtual Value* visit(ExprStatementNode* statementNode);
        virtual Value* visit(LiteralExprNode* exprNode);
        virtual Value* visit(const std::unique_ptr<ExprNode>& exprNode);
        virtual Value* visit(const std::unique_ptr<StatementNode>& statementNode);
        virtual Value* visit(CallExprNode* exprNode);
        virtual Value* visit(FunctionNode* functionNode);
        virtual void visit(const std::unique_ptr<ProgramNode>& programNode);

        std::unique_ptr<ProgramNode> astRoot;
        std::unique_ptr<LLVMContext> context;
        std::unique_ptr<Module> module;
        std::unique_ptr<IRBuilder<>> builder;

};