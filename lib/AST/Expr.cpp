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
#include "dusk/AST/ASTWalker.h"
#include "llvm/IR/Value.h"

using namespace dusk;

// MARK: - Number literal expresssion

NumberLiteralExpr::NumberLiteralExpr(int64_t V, llvm::SMRange ValL)
: Value(V), ValueLoc(ValL)
{}

llvm::SMRange NumberLiteralExpr::getSourceRange() const {
    return ValueLoc;
}

bool NumberLiteralExpr::walk(ASTWalker &Walker) {
    if (!Walker.preWalk(this))
        return false;
    if (!Walker.postWalk(this))
        return false;
    return true;
}

// MARK: - Variable expression

VariableExpr::VariableExpr(llvm::StringRef N, llvm::SMRange NL)
: Name(N), NameLoc(NL)
{}

llvm::SMRange VariableExpr::getSourceRange() const {
    return NameLoc;
}

bool VariableExpr::walk(ASTWalker &Walker) {
    if (!Walker.preWalk(this))
        return false;
    if (!Walker.postWalk(this))
        return false;
    return true;
}

// MARK: - Infix expr

InfixExpr::InfixExpr(Expr *L, Expr *R, Token O)
: LHS(L), RHS(R), Op(O)
{
    assert(LHS && RHS && "Invalid `infix` expresssion.");
}

llvm::SMRange InfixExpr::getSourceRange() const {
    return { LHS->getLocStart(), RHS->getLocEnd() };
}

bool InfixExpr::walk(ASTWalker &Walker) {
    if (!Walker.preWalk(this))
        return false;
    if (!LHS->walk(Walker))
        return false;
    if (!RHS->walk(Walker))
        return false;
    if (!Walker.postWalk(this))
        return false;
    return true;
}

// MARK: - Infix expr

AssignExpr::AssignExpr(VariableExpr *L, Expr *R)
: LHS(L), RHS(R)
{
    assert(LHS && RHS && "Invalid `assign` expression.");
}

llvm::SMRange AssignExpr::getSourceRange() const {
    return { LHS->getLocStart(), RHS->getLocEnd() };
}

bool AssignExpr::walk(ASTWalker &Walker) {
    if (!Walker.preWalk(this))
        return false;
    if (!LHS->walk(Walker))
        return false;
    if (!RHS->walk(Walker))
        return false;
    if (!Walker.postWalk(this))
        return false;
    return true;
}

// MARK: - FuncCall expr

FuncCall::FuncCall(llvm::StringRef FN, llvm::SMLoc FL, ParamList *A)
: Callee(FN), FnLoc(FL), Args(A)
{
    assert(Args && "Invalid `FuncCall` expression.");
}

llvm::SMRange FuncCall::getSourceRange() const {
    return { FnLoc, Args->getLocEnd() };
}

bool FuncCall::walk(ASTWalker &Walker) {
    if (!Walker.preWalk(this))
        return false;
    if (!Args->walk(Walker))
        return false;
    if (!Walker.postWalk(this))
        return false;
    return true;
    
}

