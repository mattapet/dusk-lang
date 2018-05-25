//===--- TypeCheckStmt.cpp ------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "TypeChecker.h"

#include "dusk/AST/Diagnostics.h"
#include "dusk/AST/Scope.h"
#include "dusk/AST/NameLookup.h"
#include "dusk/AST/ASTVisitor.h"
#include "dusk/Sema/Sema.h"

using namespace dusk;
using namespace sema;

namespace {

class DeclChecker : public DeclVisitor<DeclChecker> {
  TypeChecker &TC;

  typedef ASTVisitor super;

  friend super;

public:
  DeclChecker(TypeChecker &TC) : TC(TC) {}

private:
  void visitLetDecl(LetDecl *D) {
    if (!TC.Lookup.declareLet(D))
      TC.diagnose(D->getLocStart(), diag::redefinition_of_identifier);
    if (!D->hasValue()) {
      TC.diagnose(D->getLocEnd(), diag::expected_default_initialization);
      return;
    }

    if (D->hasTypeRepr())
      TC.typeCheckType(D->getTypeRepr());

    auto Val = TC.typeCheckExpr(D->getValue());

    if (D->hasTypeRepr())
      TC.typeCheckEquals(D->getTypeRepr()->getType(), Val->getType());
    D->setValue(Val);
    D->setType(Val->getType());
  }

  void visitVarDecl(VarDecl *D) {
    if (!TC.Lookup.declareVar(D))
      TC.diagnose(D->getLocStart(), diag::redefinition_of_identifier);
    if (D->hasTypeRepr())
      TC.typeCheckType(D->getTypeRepr());

    if (!D->hasValue()) {
      if (!D->hasTypeRepr())
        TC.diagnose(D->getLocEnd(), diag::expected_type_annotation);
      else
        D->setType(D->getTypeRepr()->getType());
      return;
    }

    auto Val = TC.typeCheckExpr(D->getValue());
    if (D->hasTypeRepr())
      TC.typeCheckEquals(D->getTypeRepr()->getType(), Val->getType());
    TC.ensureMutable(D->getValue());
    D->setValue(Val);
    D->setType(Val->getType());
  }

  void visitParamDecl(ParamDecl *D) {
    if (!TC.Lookup.declareLet(D))
      TC.diagnose(D->getLocStart(), diag::redefinition_of_identifier);
    if (D->hasTypeRepr()) {
      TC.typeCheckType(D->getTypeRepr());
      D->setType(D->getTypeRepr()->getType());
      return;
    }
  }

  void visitFuncDecl(FuncDecl *D) {
    PushScopeRAII Push(TC.ASTScope, Scope::FnProtoScope);
    TC.typeCheckPattern(D->getArgs());

    Type *ArgsTy = D->getArgs()->getType();
    Type *RetTy = TC.Ctx.getVoidType();

    // Override return type if specified
    if (D->hasTypeRepr()) {
      TC.typeCheckType(D->getTypeRepr());
      RetTy = D->getTypeRepr()->getType();
    }

    if (!ArgsTy || !RetTy)
      return;

    D->setType(new (TC.Ctx) FunctionType(ArgsTy, RetTy));
  }

  void visitModuleDecl(ModuleDecl *D) {
    for (auto N : D->getContents()) {
      if (auto D = dynamic_cast<Decl *>(N))
        typeCheckDecl(D);
      else if (auto E = dynamic_cast<Expr *>(N))
        TC.diagnose(E->getLocStart(), diag::unexpected_expresssion);
      else if (auto S = dynamic_cast<Stmt *>(N))
        TC.typeCheckStmt(S);
      else
        llvm_unreachable("Unexpected node type.");
    }
  }

public:
  void typeCheckDecl(Decl *D) { super::visit(D); }
};

} // anonymous namespace

void TypeChecker::typeCheckDecl(Decl *D) {
  DeclChecker(*this).typeCheckDecl(D);
}
