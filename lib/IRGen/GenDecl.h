//===--- GenDecl.h - Dusk IR declaration generation -------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_GEN_DECL_H
#define DUSK_GEN_DECL_H

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

class GenDecl {
  Decl *Declaration;
  Context &Ctx;

public:
  GenDecl(Decl *D, Context &C);
  
  bool gen();
  bool genArgs();
  
private:
  bool codegen(Decl *D);
  bool codegen(VarDecl *D);
  bool codegen(ConstDecl *D);
  bool codegen(FuncDecl *D);
  bool codegen(ParamDecl *D);
};

} // namespace irgen

} // namespace dusk

#endif /* DUSK_GEN_DECL_H */
