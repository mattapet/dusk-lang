
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
#include "dusk/AST/Type.h"
#include "dusk/AST/TypeRepr.h"
#include "dusk/AST/Diagnostics.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallVector.h"

#include "TypeChecker.h"


using namespace dusk;
using namespace sema;

namespace {
  
class FwdDeclarator: public ASTWalker {
  Sema &S;
  Context &Ctx;
  DiagnosticEngine &Diag;
  
public:
  FwdDeclarator(Sema &S, Context &C, DiagnosticEngine &D)
      : S(S), Ctx(C), Diag(D) {}

  virtual bool preWalk(Decl *D) override {
    if (D->getKind() == DeclKind::Func)
      return true;
    if (D->getKind() == DeclKind::Module)
      return true;
    return false;
  }
  
  virtual bool postWalk(Decl *D) override {
    if (auto FD = dynamic_cast<FuncDecl *>(D)) {
      if (!Ctx.declareFunc(D)) {
        Diag.diagnose(D->getLocStart(), diag::redefinition_of_identifier);
        return false;
      }
      D->setType(S.typeReprResolve(FD));
      return true;
    }
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

void Sema::perform() {
  declareFuncs();
  typeCheck();
}

void Sema::declareFuncs() {
  FwdDeclarator D(*this, DeclCtx, Diag);
  Ctx.getRootModule()->walk(D);
}

void Sema::typeCheck() {
  TypeChecker TC(*this, DeclCtx, Ctx, Diag);
  Ctx.getRootModule()->walk(TC);
}

static Type *typeReprResolve(ASTContext &C, StringRef Ident) {
  std::unique_ptr<Type> Ty;
  if (Ident == "Int")
    Ty = std::make_unique<IntType>();
  else if (Ident == "Void")
    Ty = std::make_unique<VoidType>();
  else
    return nullptr;
  return C.pushType(std::move(Ty));
}

Type *Sema::typeReprResolve(TypeRepr *TR) {
  switch (TR->getKind()) {
  case TypeReprKind::Ident:
    return ::typeReprResolve(Ctx, static_cast<IdentTypeRepr *>(TR)->getIdent());
  case TypeReprKind::FuncRet:
    return ::typeReprResolve(Ctx,
                             static_cast<FuncRetTypeRepr *>(TR)->getIdent());
  }
}

Type *Sema::typeReprResolve(FuncDecl *FD) {
  llvm::SmallVector<Type *, 128> Args;
  for (auto Arg : FD->getArgs()->getVars())
    Args.push_back(typeReprResolve(Arg->getTypeRepr()));
  
  auto ArgsT = std::make_unique<PatternType>(std::move(Args));
  auto ArgsTT = Ctx.pushType(std::move(ArgsT));
  
  Type *RetT = nullptr;
  if (!FD->hasTypeRepr()) {
    auto F = std::make_unique<VoidType>();
    RetT = Ctx.pushType(std::move(F));
  } else {
    RetT = typeReprResolve(FD->getTypeRepr());
  }
  
  auto Ty = std::make_unique<FunctionType>(ArgsTT, RetT);
  return Ctx.pushType(std::move(Ty));
}


