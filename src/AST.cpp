#include "AST.h"

void ProgramNode::addStatement(std::unique_ptr<StatementNode> statement)
{
    this->Statements.push_back(std::move(statement));
}

LiteralExprNode::LiteralExprNode(const std::string typeName, char *data)
    : TypeName(typeName), data(data)
{
}
