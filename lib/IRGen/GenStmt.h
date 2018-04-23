//===--- GenStmt.h - Dusk IR statement generation ---------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_GEN_STMT_H
#define DUSK_GEN_STMT_H

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

class GenDel;
class GenExpr;
class GenStmt;

class GenStmt {
  Stmt *Statement;
  Context &Ctx;

public:
  GenStmt(Stmt *S, Context &C);

  bool gen();

private:
  bool codegen(Stmt *S);
  bool codegen(BreakStmt *S);
  bool codegen(ReturnStmt *S);
  bool codegen(RangeStmt *S);
  bool codegen(SubscriptStmt *S);
  bool codegen(BlockStmt *S);
  bool codegen(ForStmt *S);
  bool codegen(WhileStmt *S);
  bool codegen(IfStmt *S);
  bool codegen(FuncStmt *S);
};

} // namespace irgen

} // namespace dusk

#endif /* DUSK_GEN_STMT_H */

