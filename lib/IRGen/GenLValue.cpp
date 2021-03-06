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
    auto Addr = IRGM.getVal(E->getName());
    return LValue::getVal(E->getType(), Addr.getAddress());
  }
  
  LValue visitSubscriptExpr(SubscriptExpr *E) {
    auto Ptr = IRGM.emitRValue(E->getBase());
    auto Idx =
    IRGM.emitRValue(E->getSubscript()->getSubscriptStmt()->getValue());
    
    return LValue::getArrayElem(E->getType(), (llvm::Value *)Ptr, Idx);
  }
  
  LValue visitInOutExpr(InOutExpr *E);
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

LValue LValueEmitter::visitInOutExpr(InOutExpr *E) {
  llvm_unreachable("Should never visit this expr");
}

LValue LValueEmitter::visitCallExpr(CallExpr *E) {
  llvm_unreachable("Should never visit this expr");
}

LValue LValueEmitter::visitNumberLiteralExpr(NumberLiteralExpr *E) {
  llvm_unreachable("Should never visit this expr");
}

LValue LValueEmitter::visitArrayLiteralExpr(ArrayLiteralExpr *E) {
  llvm_unreachable("Should never visit this expr");
}

LValue LValueEmitter::visitParenExpr(ParenExpr *E) {
  llvm_unreachable("Should never visit this expr");
}

LValue LValueEmitter::visitAssignExpr(AssignExpr *E) {
  llvm_unreachable("Should never visit this expr");
}

LValue LValueEmitter::visitInfixExpr(InfixExpr *E) {
  llvm_unreachable("Should never visit this expr");
}

LValue LValueEmitter::visitPrefixExpr(PrefixExpr *E) {
  llvm_unreachable("Should never visit this expr");
}
