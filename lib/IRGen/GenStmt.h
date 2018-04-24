//===--- GenStmt.h - Dusk IR statement generation ---------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_STMT_H
#define DUSK_IRGEN_STMT_H

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

bool codegenStmt(Context &Ctx, Scope &Scp, Stmt *S);
bool codegenStmt(Context &Ctx, Scope &Scp, BreakStmt *S);
bool codegenStmt(Context &Ctx, Scope &Scp, ReturnStmt *S);
bool codegenStmt(Context &Ctx, Scope &Scp, BlockStmt *S);
bool codegenStmt(Context &Ctx, Scope &Scp, IfStmt *S);
bool codegenStmt(Context &Ctx, Scope &Scp, WhileStmt *S);
bool codegenStmt(Context &Ctx, Scope &Scp, ForStmt *S);
bool codegenStmt(Context &Ctx, Scope &Scp, FuncStmt *S);
  
  
} // namespace irgen

} // namespace dusk

#endif /* DUSK_IRGEN_STMT_H */

