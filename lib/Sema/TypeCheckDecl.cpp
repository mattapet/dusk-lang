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
  void visitVarDecl(VarDecl *D) {
    if (D->hasTypeRepr())
      TC.typeCheckType(D->getTypeRepr());

    if (!D->hasValue()) {
      // All constant must be initialized explicitelly with a value.
      if (D->isLet()) {
        TC.diagnose(D->getLocEnd(), diag::expected_default_initialization);
        return;
      }
      
      // If we don't have value nor explicit type, we cannot resolve the type
      // of the variable.
      if (!D->hasTypeRepr())
        return TC.diagnose(D->getLocEnd(), diag::expected_type_annotation);
      else
        D->setType(D->getTypeRepr()->getType());
      
      // Phisically declare just before leaving the method.
      // It allowes users to declare a variable with the same name as the old
      // any one in parent scope.
      if (!TC.Lookup.declareVar(D))
        TC.diagnose(D->getLocStart(), diag::redefinition_of_identifier);
      return;
    }

    auto Val = TC.typeCheckExpr(D->getValue());
    if (!Val->getType())
      return;
    
    if (D->hasTypeRepr() && D->getTypeRepr()->getType())
      // If we have an explicit type, check if it matches the value.
      if (!TC.typeCheckEquals(D->getTypeRepr()->getType(), Val->getType()))
        return TC.diagnose(D->getValue()->getLocStart(), diag::type_missmatch);

    if (!D->hasTypeRepr() && !Val->getType()->isValueType())
      return TC.diagnose(D->getValue()->getLocStart(),
                         diag::expected_value_type_expression);

    // Check for discarting reference object.
    if (D->isLet() && Val->getType()->isRefType())
      TC.ensureMutable(D->getValue());
    
    // Finally we can ifer the value type. Either we don't have an explicit
    // type or types are equal.
    D->setValue(Val);
    D->setType(Val->getType());
    
    // Phisically declare just before leaving the method.
    // It allowes users to declare a variable with the same name as the old
    // any one in parent scope.
    if (!TC.Lookup.declareVar(D))
      TC.diagnose(D->getLocStart(), diag::redefinition_of_identifier);
  }

  void visitParamDecl(ParamDecl *D) {
    if (!TC.Lookup.declareVar(D))
      TC.diagnose(D->getLocStart(), diag::redefinition_of_identifier);
    
    if (D->hasTypeRepr()) {
      TC.typeCheckType(D->getTypeRepr());
      D->setType(D->getTypeRepr()->getType());
    }
    
    if (auto InOut = dynamic_cast<InOutType *>(D->getType()))
      D->setSpecifier(ValDecl::Specifier::InOut);
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
        TC.diagnose(E->getLocStart(), diag::unexpected_global_expresssion);
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
