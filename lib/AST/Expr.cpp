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
#include "dusk/AST/Pattern.h"

using namespace dusk;

// MARK: - Number literal expresssion

NumberLiteralExpr::NumberLiteralExpr(int V, llvm::SMRange ValL)
: Expr(ExprKind::NumberLiteral), Value(V), ValueLoc(ValL)
{}

llvm::SMRange NumberLiteralExpr::getSourceRange() const {
    return ValueLoc;
}

// MARK: - Identifier expression

IdentifierExpr::IdentifierExpr(llvm::StringRef N, llvm::SMLoc L)
: Expr(ExprKind::Identifier), Name(N), NameLoc(L)
{}

llvm::SMRange IdentifierExpr::getSourceRange() const {
    auto E = llvm::SMLoc::getFromPointer(Name.data() + Name.size());
    return { NameLoc, E };
}

// MARK: - Binary expression

BinrayExpr::BinrayExpr(Expr *L, Expr *R, Token O)
: Expr(ExprKind::Binary), LHS(L), RHS(R), Op(O)
{
    assert(LHS && RHS && "Invalid `infix` expresssion.");
}

llvm::SMRange BinrayExpr::getSourceRange() const {
    return { LHS->getLocStart(), RHS->getLocEnd() };
}

// MARK: - Infix expression

AssignExpr::AssignExpr(Expr *L, Expr *R)
: Expr(ExprKind::Assign), Dest(L), Source(R)
{
    assert(Dest && Source && "Invalid `assign` expression.");
}

llvm::SMRange AssignExpr::getSourceRange() const {
    return { Dest->getLocStart(), Source->getLocEnd() };
}

// MARK: - Unary expresssion

UnaryExpr::UnaryExpr(Expr *D, Token O)
: Expr(ExprKind::Unary), Dest(D), Op(O)
{
    assert(Dest && "Invalid `unary` expression.");
}

llvm::SMRange UnaryExpr::getSourceRange() const {
    return { Op.getLoc(), Dest->getLocEnd() };
}

// MARK: - FuncCall expression

CallExpr::CallExpr(IdentifierExpr *C, ExprPattern *A)
: Expr(ExprKind::Call), Callee(C), Args(A)
{
    assert(C && Args && "Invalid `FuncCall` expression.");
}

llvm::SMRange CallExpr::getSourceRange() const {
    return { Callee->getLocStart(), Args->getLocEnd() };
}

// MARK: - Subscript expression

SubscriptExpr::SubscriptExpr(IdentifierExpr *B, SubscriptPattern *S)
: Expr(ExprKind::Subscript), Base(B), Subscript(S)
{}

llvm::SMRange SubscriptExpr::getSourceRange() const {
    return { Base->getLocStart(), Subscript->getLocEnd() };
}

