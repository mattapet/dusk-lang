//===--- GenExpr.h - Dusk IR expresssion generation -------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_EXPR_H
#define DUSK_IRGEN_EXPR_H

namespace llvm {
  class Value;
}

namespace dusk {
class Expr;
class NumberLiteralExpr;
class IdentifierExpr;
class InfixExpr;
class PrefixExpr;
class AssignExpr;
class CallExpr;
  
namespace irgen {
class Context;
  
llvm::Value *codegenExpr(Context &Ctx, Expr *E);
llvm::Value *codegenExpr(Context &Ctx, NumberLiteralExpr *E);
llvm::Value *codegenExpr(Context &Ctx, IdentifierExpr *E);
llvm::Value *codegenExpr(Context &Ctx, InfixExpr *E);
llvm::Value *codegenExpr(Context &Ctx, PrefixExpr *E);
llvm::Value *codegenExpr(Context &Ctx, AssignExpr *E);
llvm::Value *codegenExpr(Context &Ctx, CallExpr *E);
  
} // namespace irgen
  
} // namespace dusk

#endif /* DUSK_IRGEN_EXPR_H */

