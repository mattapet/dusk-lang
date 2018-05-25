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
#include "dusk/AST/Type.h"
#include "dusk/AST/TypeRepr.h"

using namespace dusk;

Expr::Expr(ExprKind K) : Kind(K), Ty(nullptr), Solved(false) {}

#define EXPR(CLASS, PARENT)                                                    \
CLASS##Expr *Expr::get##CLASS##Expr() {                                        \
  assert(Kind == ExprKind::CLASS && "Invalid Expr convertion");                \
  return static_cast<CLASS##Expr *>(this);                                     \
}
#include "dusk/AST/ExprNodes.def"

// MARK: - Number literal expresssion

NumberLiteralExpr::NumberLiteralExpr(int64_t V, SMRange ValL)
    : Expr(ExprKind::NumberLiteral), Value(V), ValueLoc(ValL) {}

SMRange NumberLiteralExpr::getSourceRange() const { return ValueLoc; }

// MARK: - Array literal expression

ArrayLiteralExpr::ArrayLiteralExpr(Pattern *V)
    : Expr(ExprKind::ArrayLiteral), Values(V) {}

SMRange ArrayLiteralExpr::getSourceRange() const {
  return Values->getSourceRange();
}

// MARK: - Identifier expression

IdentifierExpr::IdentifierExpr(StringRef N, SMLoc L)
    : Expr(ExprKind::Identifier), Name(N), NameLoc(L) {}

SMRange IdentifierExpr::getSourceRange() const {
  auto E = SMLoc::getFromPointer(Name.data() + Name.size());
  return {NameLoc, E};
}

// MARK: - Parenthesis expression

ParenExpr::ParenExpr(Expr *E, SMLoc L, SMLoc R)
    : Expr(ExprKind::Paren), Expression(E), LPar(L), RPar(R) {}

SMRange ParenExpr::getSourceRange() const { return {LPar, RPar}; }

// MARK: - Infix expression

InfixExpr::InfixExpr(Expr *L, Expr *R, Token O)
    : Expr(ExprKind::Infix), LHS(L), RHS(R), Op(O) {}

SMRange InfixExpr::getSourceRange() const {
  return {LHS->getLocStart(), RHS->getLocEnd()};
}

// MARK: - Infix expression

AssignExpr::AssignExpr(Expr *L, Expr *R)
    : Expr(ExprKind::Assign), Dest(L), Source(R) {}

SMRange AssignExpr::getSourceRange() const {
  return {Dest->getLocStart(), Source->getLocEnd()};
}

// MARK: - Unary expresssion

PrefixExpr::PrefixExpr(Expr *D, Token O)
    : Expr(ExprKind::Prefix), Dest(D), Op(O) {}

SMRange PrefixExpr::getSourceRange() const {
  return {Op.getLoc(), Dest->getLocEnd()};
}

// MARK: - FuncCall expression

CallExpr::CallExpr(Expr *C, Pattern *A)
    : Expr(ExprKind::Call), Callee(C), Args(A) {}

SMRange CallExpr::getSourceRange() const {
  return {Callee->getLocStart(), Args->getLocEnd()};
}

// MARK: - Subscript expression

SubscriptExpr::SubscriptExpr(Expr *B, Stmt *S)
    : Expr(ExprKind::Subscript), Base(B), Subscript(S) {}

SMRange SubscriptExpr::getSourceRange() const {
  return {Base->getLocStart(), Subscript->getLocEnd()};
}
