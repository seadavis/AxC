#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/IRBuilder.h"
#include "TreeWalker.h"

TreeWalker::TreeWalker(std::unique_ptr<ProgramNode> ast) 
: astRoot(std::move(ast))
{
    context = std::make_unique<LLVMContext>();
    module = std::make_unique<Module>("module1", *context);
  
    // Create a new builder for the module.
    builder = std::make_unique<IRBuilder<>>(*context);

    Type* Int8Type = Type::getInt8Ty(*context);

    // Get explicit i8* type
    PointerType* Int8PtrType = PointerType::get(Int8Type, 0);

    FunctionType *funcType = FunctionType::get(builder->getInt32Ty(), {Int8PtrType}, false);
    Function *printMessageFunc = Function::Create(
        funcType, Function::ExternalLinkage, "puts", *module);
    printMessageFunc->dump();
}

Module* TreeWalker::generateIR()
{
    visit(astRoot);
    return module.get();
}

llvm::Value* TreeWalker::visit(ExprStatementNode* statementNode)
{
    return visit(statementNode->Expr);
}

llvm::Value*  TreeWalker::visit(LiteralExprNode* exprNode)
{
    return builder->CreateGlobalStringPtr(exprNode->data);
}

Value*  TreeWalker::visit(CallExprNode* exprNode)
{
    //this is where we get the puts type to a global string ptr
    Function* calleeF = module->getFunction(exprNode->FunctionName);
    std::vector<Value *> argsV;
    for (size_t i = 0; i < exprNode->Arguments.size(); i++) {
        argsV.push_back(visit(exprNode->Arguments[i]));
    }
    return builder->CreateCall(calleeF, argsV);
}

Value* TreeWalker::visit(FunctionNode* functionNode)
{
    std::vector<Type *> t;
    FunctionType *ft =
        FunctionType::get(Type::getInt8Ty(*context), t, false);

    Function *f =
        Function::Create(ft, Function::ExternalLinkage, functionNode->Proto.get()->Name, *module);

    BasicBlock *bb = BasicBlock::Create(*context, "entry", f);
    builder->SetInsertPoint(bb);

    for(int i = 0; i < functionNode->Body.size(); i++)
    {
        visit(functionNode->Body[i]);
    }

    builder->CreateRet(llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), 0));

    return f;
}

Value* TreeWalker::visit(const std::unique_ptr<ExprNode>& exprNode)
{
    if (LiteralExprNode* literalNode = dynamic_cast<LiteralExprNode*>(exprNode.get())) {
       return visit(literalNode);
    }
    if (CallExprNode* callNode = dynamic_cast<CallExprNode*>(exprNode.get())) {
        return visit(callNode);
    }
}

Value* TreeWalker::visit(const std::unique_ptr<StatementNode>& statementNode)
{

    if (ExprStatementNode* exprStatementNode = dynamic_cast<ExprStatementNode*>(statementNode.get())) {
        return visit(exprStatementNode);
     }
     if (FunctionNode* functionNode = dynamic_cast<FunctionNode*>(statementNode.get())) {
         return visit(functionNode);
      }
}

void TreeWalker::visit( const std::unique_ptr<ProgramNode>& programNode)
{
    for(int i = 0; i < programNode->Statements.size(); i++)
    {
        visit(programNode->Statements[i]);
    }
}
