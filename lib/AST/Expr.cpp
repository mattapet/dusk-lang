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
    : Expr(ExprKind::NumberLiteral), Value(V), ValueLoc(ValL) {}

llvm::SMRange NumberLiteralExpr::getSourceRange() const { return ValueLoc; }

// MARK: - Identifier expression

IdentifierExpr::IdentifierExpr(llvm::StringRef N, llvm::SMLoc L)
    : Expr(ExprKind::Identifier), Name(N), NameLoc(L) {}

llvm::SMRange IdentifierExpr::getSourceRange() const {
  auto E = llvm::SMLoc::getFromPointer(Name.data() + Name.size());
  return {NameLoc, E};
}

// MARK: - Parenthesis expression

ParenExpr::ParenExpr(Expr *E, llvm::SMLoc L, llvm::SMLoc R)
    : Expr(ExprKind::Paren), Expression(E), LPar(L), RPar(R) {}

llvm::SMRange ParenExpr::getSourceRange() const { return {LPar, RPar}; }

// MARK: - Infix expression

InfixExpr::InfixExpr(Expr *L, Expr *R, Token O)
    : Expr(ExprKind::Infix), LHS(L), RHS(R), Op(O) {}

llvm::SMRange InfixExpr::getSourceRange() const {
  return {LHS->getLocStart(), RHS->getLocEnd()};
}

// MARK: - Infix expression

AssignExpr::AssignExpr(Expr *L, Expr *R)
    : Expr(ExprKind::Assign), Dest(L), Source(R) {}

llvm::SMRange AssignExpr::getSourceRange() const {
  return {Dest->getLocStart(), Source->getLocEnd()};
}

// MARK: - Unary expresssion

PrefixExpr::PrefixExpr(Expr *D, Token O)
    : Expr(ExprKind::Prefix), Dest(D), Op(O) {}

llvm::SMRange PrefixExpr::getSourceRange() const {
  return {Op.getLoc(), Dest->getLocEnd()};
}

// MARK: - FuncCall expression

CallExpr::CallExpr(Expr *C, Pattern *A)
    : Expr(ExprKind::Call), Callee(C), Args(A) {}

llvm::SMRange CallExpr::getSourceRange() const {
  return {Callee->getLocStart(), Args->getLocEnd()};
}

// MARK: - Subscript expression

SubscriptExpr::SubscriptExpr(Expr *B, Pattern *S)
    : Expr(ExprKind::Subscript), Base(B), Subscript(S) {}

llvm::SMRange SubscriptExpr::getSourceRange() const {
  return {Base->getLocStart(), Subscript->getLocEnd()};
}
