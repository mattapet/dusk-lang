//===--- Expr.cpp ---------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/Expr.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/Stmt.h"

using namespace dusk;

// MARK: - Number literal expresssion

NumberLiteralExpr::NumberLiteralExpr(int V, llvm::SMRange ValL)
: Expr(ExprKind::NumberLiteral), Value(V), ValueLoc(ValL)
{}

llvm::SMRange NumberLiteralExpr::getSourceRange() const {
    return ValueLoc;
}

// MARK: - Variable expression

VariableExpr::VariableExpr(llvm::StringRef N, llvm::SMLoc NL)
: Expr(ExprKind::Variable), Name(N), NameLoc(NL)
{}

llvm::SMRange VariableExpr::getSourceRange() const {
    auto End = Name.data() + Name.size();
    return { NameLoc, llvm::SMLoc::getFromPointer(End) };
}

// MARK: - Infix expr

InfixExpr::InfixExpr(Expr *L, Expr *R, Token O)
: Expr(ExprKind::Infix), LHS(L), RHS(R), Op(O)
{
    assert(LHS && RHS && "Invalid `infix` expresssion.");
}

llvm::SMRange InfixExpr::getSourceRange() const {
    return { LHS->getLocStart(), RHS->getLocEnd() };
}

// MARK: - Infix expr

AssignExpr::AssignExpr(VariableExpr *L, Expr *R)
: Expr(ExprKind::Assign), LHS(L), RHS(R)
{
    assert(LHS && RHS && "Invalid `assign` expression.");
}

llvm::SMRange AssignExpr::getSourceRange() const {
    return { LHS->getLocStart(), RHS->getLocEnd() };
}

// MARK: - FuncCall expr

FuncCall::FuncCall(llvm::StringRef FN, llvm::SMLoc FL, ParamList *A)
: Expr(ExprKind::FuncCall), Callee(FN), FnLoc(FL), Args(A)
{
    assert(Args && "Invalid `FuncCall` expression.");
}

llvm::SMRange FuncCall::getSourceRange() const {
    return { FnLoc, Args->getLocEnd() };
}

