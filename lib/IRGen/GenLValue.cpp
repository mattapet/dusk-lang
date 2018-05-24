//===--- GenLValue.cpp - L value emission -----------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/Expr.h"
#include "dusk/AST/ASTVisitor.h"

#include "IRGenModule.h"
#include "IRGenValue.h"

using namespace dusk;
using namespace irgen;

namespace {

/// Performs an rvalue emission.
class LValueEmitter : public ExprVisitor<LValueEmitter, LValue> {
  IRGenModule &IRGM;

  typedef ASTVisitor super;

  friend super;

public:
  LValueEmitter(IRGenModule &IRGM) : IRGM(IRGM) {}

private:
  LValue visitIdentifierExpr(IdentifierExpr *E) {
    return {};
  }
  
  LValue visitSubscriptExpr(SubscriptExpr *E) {
    return {};
  }
  
  LValue visitCallExpr(CallExpr *E);
  LValue visitNumberLiteralExpr(NumberLiteralExpr *E);
  LValue visitArrayLiteralExpr(ArrayLiteralExpr *E);
  LValue visitParenExpr(ParenExpr *E);
  LValue visitAssignExpr(AssignExpr *E);
  LValue visitInfixExpr(InfixExpr *E);
  LValue visitPrefixExpr(PrefixExpr *E);

public:
  LValue emitLValue(Expr *E) { return super::visit(E); }
};

} // anonymous namespace

LValue IRGenModule::emitLValue(Expr *E) {
  return LValueEmitter(*this).emitLValue(E);
}

// MARK: - Invalid emittion methods implementations

LValue LValueEmitter::visitCallExpr(CallExpr *E) {
  llvm_unreachable("Should never visit parent expr");
}

LValue LValueEmitter::visitNumberLiteralExpr(NumberLiteralExpr *E) {
  llvm_unreachable("Should never visit parent expr");
}

LValue LValueEmitter::visitArrayLiteralExpr(ArrayLiteralExpr *E) {
  llvm_unreachable("Should never visit parent expr");
}

LValue LValueEmitter::visitParenExpr(ParenExpr *E) {
  llvm_unreachable("Should never visit parent expr");
}

LValue LValueEmitter::visitAssignExpr(AssignExpr *E) {
  llvm_unreachable("Should never visit parent expr");
}

LValue LValueEmitter::visitInfixExpr(InfixExpr *E) {
  llvm_unreachable("Should never visit parent expr");
}

LValue LValueEmitter::visitPrefixExpr(PrefixExpr *E) {
  llvm_unreachable("Should never visit parent expr");
}
