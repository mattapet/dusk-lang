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
#include "dusk/AST/NameLookup.h"

#include "dusk/Strings.h"

#include "dusk/Runtime/RuntimeFuncs.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallVector.h"

#include "TypeChecker.h"

using namespace dusk;
using namespace sema;

namespace {

class FwdDeclarator : public ASTWalker {
  Sema &S;
  NameLookup &Ctx;
  DiagnosticEngine &Diag;

public:
  FwdDeclarator(Sema &S, NameLookup &C, DiagnosticEngine &D)
      : S(S), Ctx(C), Diag(D) {}

  bool preWalkDecl(Decl *D) override {
    if (D->getKind() == DeclKind::Func)
      return true;
    if (D->getKind() == DeclKind::Module)
      return true;
    return false;
  }

  bool postWalkDecl(Decl *D) override {
    if (auto FD = dynamic_cast<FuncDecl *>(D)) {
      if (!Ctx.declareFunc(D)) {
        Diag.diagnose(D->getLocStart(), diag::redefinition_of_identifier);
        return false;
      }
      D->setType(S.typeReprResolve(FD));
    }
    return true;
  }

  // Skip all expressions
  std::pair<bool, Expr *> preWalkExpr(Expr *E) override { return {false, E}; }
  // Skip all patterns
  bool preWalkPattern(Pattern *P) override { return false; }
  // Skip all types
  bool preWalkTypeRepr(TypeRepr *TR) override { return false; }

  bool preWalkStmt(Stmt *S) override {
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
  TypeChecker(*this, DeclCtx, Ctx, Diag).typeCheckDecl(Ctx.getRootModule());
}

static Type *typeReprResolve(Sema &S, ASTContext &C, IdentTypeRepr *TyRepr) {
  if (TyRepr->getIdent() == BUILTIN_TYPE_NAME_INT)
    return new (C) IntType();
  else if (TyRepr->getIdent() == BUILTIN_TYPE_NAME_VOID)
    return new (C) VoidType();
  else
    return nullptr;
}

static Type *typeReprResolve(Sema &S, ASTContext &C, ArrayTypeRepr *TyRepr) {
  auto BaseTy = S.typeReprResolve(TyRepr->getBaseTyRepr());
  auto Size = static_cast<SubscriptStmt *>(TyRepr->getSize());
  auto SizeVal = static_cast<NumberLiteralExpr *>(Size->getValue())->getValue();
  return new (C) ArrayType(BaseTy, SizeVal);
}

static Type *typeReprResolve(Sema &S, ASTContext &C, InOutTypeRepr *TyRepr) {
  auto BaseTy = S.typeReprResolve(TyRepr->getBaseTyRepr());
  return new (C) InOutType(BaseTy);
}

Type *Sema::typeReprResolve(TypeRepr *TR) {
  switch (TR->getKind()) {
  case TypeReprKind::Ident:
    return ::typeReprResolve(*this, Ctx, static_cast<IdentTypeRepr *>(TR));
  case TypeReprKind::Array:
    return ::typeReprResolve(*this, Ctx, static_cast<ArrayTypeRepr *>(TR));
  default:
    return ::typeReprResolve(*this, Ctx, static_cast<InOutTypeRepr *>(TR));
  }
}

Type *Sema::typeReprResolve(FuncDecl *FD) {
  // Aggregate args types
  llvm::SmallVector<Type *, 128> Args;
  for (auto Arg : FD->getArgs()->getVars())
    Args.push_back(typeReprResolve(Arg->getTypeRepr()));

  auto ArgsT = new (Ctx) PatternType(std::move(Args));

  // Resolve return type
  Type *RetT = nullptr;
  if (!FD->hasTypeRepr()) {
    RetT = new (Ctx) VoidType();
  } else {
    RetT = typeReprResolve(FD->getTypeRepr());
  }

  return new (Ctx) FunctionType(ArgsT, RetT);
}

Type *Sema::typeReprResolve(ArrayLiteralExpr *E) {
  auto Ty = static_cast<PatternType *>(E->getValues()->getType());
  if (Ty->getItems().size() == 0)
    return nullptr;

  auto RefT = Ty->getItems()[0];
  auto ATy = new (Ctx) ArrayType(RefT, E->getValues()->count());
  for (size_t i = 1; i < Ty->getItems().size(); i++) {
    if (!RefT->isClassOf(Ty->getItems()[i])) {
      Diag.diagnose(E->getLocStart(), diag::array_element_mismatch);
      return nullptr;
    }
  }
  return ATy;
}
