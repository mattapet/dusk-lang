//===--- GenDecl.h - Dusk IR declaration generation -------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_DECL_H
#define DUSK_IRGEN_DECL_H

#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/IRGen/Context.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/LLVMContext.h"
#include "GenExpr.h"

namespace dusk {

namespace irgen {
  
bool codegenDecl(Context &Ctx, Decl *D);
bool codegenDecl(Context &Ctx, VarDecl *D);
bool codegenDecl(Context &Ctx, LetDecl *D);
bool codegenDecl(Context &Ctx, FuncDecl *D);
bool codegenDecl(Context &Ctx, ParamDecl *D);
  
} // namespace irgen

} // namespace dusk

#endif /* DUSK_IRGEN_DECL_H */
