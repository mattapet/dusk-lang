//===--- GenExpr.h - Dusk IR expresssion generation -------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_GEN_EXPR_H
#define DUSK_GEN_EXPR_H

#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/IRGen/Context.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/LLVMContext.h"

namespace dusk {
  
namespace irgen {

class GenExpr {
  Expr *Root;
  Context &Ctx;
  
public:
  GenExpr(Expr *R, Context &C);
  
  llvm::Value *gen();
  
private:
  llvm::Value *codegen(Expr *E);
  llvm::Value *codegen(NumberLiteralExpr *E);
  llvm::Value *codegen(IdentifierExpr *E);
  llvm::Value *codegen(InfixExpr *E);
  llvm::Value *codegen(PrefixExpr *E);
  llvm::Value *codegen(AssignExpr *E);
  llvm::Value *codegen(CallExpr *E);
};
  
} // namespace irgen
  
} // namespace dusk

#endif /* DUSK_GEN_EXPR_H */

