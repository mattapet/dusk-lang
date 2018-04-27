//===--- Sema.cpp ---------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Sema/Sema.h"

#include "dusk/AST/ASTWalker.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"

#include "TypeChecker.h"

using namespace dusk;
using namespace sema;

namespace {
  
class FwdDeclarator: public ASTWalker {
  Context &Ctx;
  
public:
  FwdDeclarator(Context &C) : Ctx(C) {}
  
  virtual bool preWalk(Decl *D) override {
    if (D->getKind() == DeclKind::Func)
      return true;
    else
      return false;
  }
  
  virtual bool postWalk(Decl *D) override {
    if (!Ctx.declareFunc(D))
      return false;
    return true;
  }
  
  // Skip all expressions.
  virtual bool preWalk(Expr *E) override { return false; }
  virtual bool preWalk(Stmt *S) override {
    switch (S->getKind()) {
      case StmtKind::Func:
      case StmtKind::Extern:
        return true;
        
      // Skip all non-func related statements.
      default:
        return false;
    }
  }
};
  
} // anonymous namespace


Sema::Sema(ASTContext &C, DiagnosticEngine &D) : Ctx(C), Diag(D) {}

void Sema::declareFuncs() {
  FwdDeclarator D{DeclCtx};
  Ctx.getRootModule()->walk(D);
}

void Sema::typeCheck() {
  
}
