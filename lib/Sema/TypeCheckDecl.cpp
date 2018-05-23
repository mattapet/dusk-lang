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
#include "dusk/Sema/TypeResolver.h"

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
    TC.Lookup.declareLet(D);
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
    TC.Lookup.declareVar(D);
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
    
    D->setValue(Val);
    D->setType(Val->getType());
  }
  
  void visitParamDecl(ParamDecl *D) {
    if (D->hasTypeRepr()) {
      TC.typeCheckType(D->getTypeRepr());
      D->setType(D->getTypeRepr()->getType());
      return;
    }
  }
  
  void visitFuncDecl(FuncDecl *D) {
    PushScopeRAII Push(TC.ASTScope, Scope::FnProtoScope);
    TC.Lookup.declareFunc(D);
    TC.typeCheckPattern(D->getArgs());
    if (D->hasTypeRepr())
      TC.typeCheckType(D->getTypeRepr());
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
  void typeCheckDecl(Decl *D) {
    if (TC.Lookup.contains(D->getName()))
      TC.diagnose(D->getLocStart(), diag::redefinition_of_identifier);
    
    super::visit(D);
  }
};

} // anonymous namespace 

void TypeChecker::typeCheckDecl(Decl *D) {
  DeclChecker(*this).typeCheckDecl(D);
}

//bool TypeChecker::preWalkLetDecl(LetDecl *D) {
//  // Check for initialization value
//  if (!D->hasValue()) {
//    diagnose(D->getLocStart(), diag::expected_default_initialization);
//    return false;
//  }
//
//  if (D->hasTypeRepr())
//    D->setType(S.typeReprResolve(D->getTypeRepr()));
//  return true;
//}
//
//bool TypeChecker::preWalkVarDecl(VarDecl *D) {
//  if (!D->hasValue() && !D->hasTypeRepr()) {
//    diagnose(D->getLocEnd(), diag::expected_type_specifier);
//    return false;
//  }
//
//  if (D->hasTypeRepr())
//    D->setType(S.typeReprResolve(D->getTypeRepr()));
//  return true;
//}
//
//bool TypeChecker::preWalkParamDecl(ParamDecl *D) {
//  if (D->hasTypeRepr())
//    D->setType(S.typeReprResolve(D->getTypeRepr()));
//  return false;
//}
//
//bool TypeChecker::preWalkFuncDecl(FuncDecl *D) {
//  D->setType(S.typeReprResolve(D));
//  return true;
//}
//
//bool TypeChecker::preWalkModuleDecl(ModuleDecl *D) { return true; }
//
//bool TypeChecker::postWalkLetDecl(LetDecl *D) {
//  // Infer type
//  if (!D->getType())
//    D->setType(D->getValue()->getType());
//
//  // Check if resolved both types
//  if (!D->getType() || !D->getValue()->getType())
//    return false;
//
//  if (!D->getType()->isValueType()) {
//    diagnose(D->getValue()->getLocStart(),
//             diag::expected_value_type_expression);
//    return false;
//  }
//
//  // Validate types
//  if (D->getType()->isClassOf(D->getValue()->getType())) {
//    // If types match, declare
//    if (Lookup.declareLet(D)) {
//      return true;
//    } else {
//      diagnose(D->getLocStart(), diag::redefinition_of_identifier);
//      return false;
//    }
//  }
//
//  diagnose(D->getValue()->getLocStart(), diag::type_missmatch);
//  return false;
//}
//
//bool TypeChecker::postWalkVarDecl(VarDecl *D) {
//  // Infer type
//  if (!D->getType() && D->hasValue())
//    D->setType(D->getValue()->getType());
//
//  // Check if resolved both types
//  if (!D->getType() || (D->hasValue() && !D->getValue()->getType()))
//    return false;
//
//  if (!D->getType()->isValueType()) {
//    diagnose(D->getValue()->getLocStart(),
//             diag::expected_value_type_expression);
//    return false;
//  }
//
//  // Validate types
//  if (!D->hasValue() || D->getType()->isClassOf(D->getValue()->getType()))
//    // If types match, declare
//    return Lookup.declareVar(D);
//
//  diagnose(D->getValue()->getLocStart(), diag::type_missmatch);
//  return false;
//}
//
//bool TypeChecker::postWalkParamDecl(ParamDecl *D) {
//  return D->getType() != nullptr;
//}
//
//bool TypeChecker::postWalkFuncDecl(FuncDecl *D) {
//  return (D->getType()) != nullptr;
//}
//
//bool TypeChecker::postWalkModuleDecl(ModuleDecl *D) { return true; }
