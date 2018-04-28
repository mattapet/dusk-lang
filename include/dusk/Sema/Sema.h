//===--- Sema.h - Dusk semantic analysis ------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_SEMA_SEMA_H
#define DUSK_SEMA_SEMA_H

#include "dusk/AST/ASTContext.h"
#include "dusk/AST/ASTNode.h"
#include "dusk/AST/Diagnostics.h"
#include "dusk/Sema/Context.h"
#include "dusk/Sema/Scope.h"

namespace dusk {
class ASTContext;
class Decl;
class Expr;
class Stmt;
class Pattern;
class FuncDecl;
class Type;
class TypeRepr;
  
namespace sema {
class TypeChecker;
  
class Sema {
  ASTContext &Ctx;
  DiagnosticEngine &Diag;
  
  Context DeclCtx;
  Scope Scp;
  
public:
  Sema(ASTContext &C, DiagnosticEngine &D);
  
  void perform();
  
  Type *typeReprResolve(TypeRepr *TR);
  Type *typeReprResolve(FuncDecl *FD);
  
private:
  void declareFuncs();
  void typeCheck();
};
  
} // namespace sema
  
} // namespace dusk

#endif /* DUSK_SEMA_SEMA_H */
