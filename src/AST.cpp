#include "AST.h"

LiteralExprNode::LiteralExprNode(const std::string typeName, char *data)
    : TypeName(typeName), data(data)
{
}


TreeWalker::TreeWalker(ProgramNode* ast) 
: astRoot(ast)
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
    for(int i = 0; i < astRoot->Statements.size(); i++)
    {
       astRoot->Statements[i]->Accept(this);
    }

    return module.get();
}

llvm::Value* TreeWalker::visit(ExprStatementNode* statementNode)
{
    return statementNode->Expr->Accept(this);
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
        argsV.push_back(exprNode->Arguments[i]->Accept(this));
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
       functionNode->Body[i]->Accept(this);
    }

    builder->CreateRet(llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), 0));

    return f;
}
