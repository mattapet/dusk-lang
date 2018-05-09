//===--- GenExpr.h - Dusk IR expresssion generation -------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_GEN_EXPR_H
#define DUSK_IRGEN_GEN_EXPR_H

#include "llvm/IR/IRBuilder.h"

namespace llvm {
  class Value;
}

namespace dusk {
class Expr;
class NumberLiteralExpr;
class ArrayLiteralExpr;
class IdentifierExpr;
class SubscriptExpr;
class InfixExpr;
class PrefixExpr;
class AssignExpr;
class CallExpr;


namespace irgen {
class IRGenModule;
class IRGenFunc;

llvm::Value *codegenExpr(IRGenModule &IRGM, NumberLiteralExpr *E);
llvm::Value *codegenExpr(IRGenModule &IRGM, ArrayLiteralExpr *E);
llvm::Value *codegenExpr(IRGenModule &IRGM, IdentifierExpr *E);
llvm::Value *codegenExpr(IRGenModule &IRGM, SubscriptExpr *E);
llvm::Value *codegenExpr(IRGenModule &IRGM, InfixExpr *E);
llvm::Value *codegenExpr(IRGenModule &IRGM, PrefixExpr *E);
llvm::Value *codegenExpr(IRGenModule &IRGM, AssignExpr *E);
llvm::Value *codegenExpr(IRGenModule &IRGM, CallExpr *E);
llvm::Value *codegenExpr(IRGenModule &IRGM, Expr *E);

} // namespace irgen

} // namespace dusk

#endif /* DUSK_IRGEN_GEN_EXPR_H */

