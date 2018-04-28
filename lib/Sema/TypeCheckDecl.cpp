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
#include "dusk/Sema/Sema.h"
#include "dusk/Sema/Context.h"

using namespace dusk;
using namespace sema;

bool TypeChecker::preWalkLetDecl(LetDecl *D) {
  // Check for initialization value
  if (!D->hasValue()) {
    diagnose(D->getLocStart(), diag::expected_default_initialization);
    return false;
  }

  if (D->hasTypeRepr())
    D->setType(S.typeReprResolve(D->getTypeRepr()));
  return true;
}

bool TypeChecker::preWalkVarDecl(VarDecl *D) {
  if (!D->hasValue() && !D->hasTypeRepr()) {
    diagnose(D->getLocEnd(), diag::expected_type_specifier);
    return false;
  }

  if (D->hasTypeRepr())
    D->setType(S.typeReprResolve(D->getTypeRepr()));
  return true;
}

bool TypeChecker::preWalkParamDecl(ParamDecl *D) {
  if (D->hasTypeRepr())
    D->setType(S.typeReprResolve(D->getTypeRepr()));
  return false;
}

bool TypeChecker::preWalkFuncDecl(FuncDecl *D) {
  D->setType(S.typeReprResolve(D));
  return true;
}

bool TypeChecker::preWalkModuleDecl(ModuleDecl *D) { return true; }

bool TypeChecker::postWalkLetDecl(LetDecl *D) {
  // Infer type
  if (!D->getType())
    D->setType(D->getValue()->getType());

  // Check if resolved both types
  if (!D->getType() || !D->getValue()->getType())
    return false;

  // Validate types
  if (D->getType()->isClassOf(D->getValue()->getType())) {
    // If types match, declare
    if (DeclCtx.declareLet(D)) {
      return true;
    } else {
      diagnose(D->getLocStart(), diag::redefinition_of_identifier);
      return false;
    }
  }

  diagnose(D->getValue()->getLocStart(), diag::type_missmatch);
  return false;
}

bool TypeChecker::postWalkVarDecl(VarDecl *D) {
  // Infer type
  if (!D->getType() && D->hasValue())
    D->setType(D->getValue()->getType());

  // Check if resolved both types
  if (!D->getType() || (D->hasValue() && !D->getValue()->getType()))
    return false;

  // Validate types
  if (!D->hasValue() || D->getType()->isClassOf(D->getValue()->getType()))
    // If types match, declare
    return DeclCtx.declareVar(D);

  diagnose(D->getValue()->getLocStart(), diag::type_missmatch);
  return false;
}

bool TypeChecker::postWalkParamDecl(ParamDecl *D) {
  return D->getType() != nullptr;
}

bool TypeChecker::postWalkFuncDecl(FuncDecl *D) {
  return (D->getType()) != nullptr;
}

bool TypeChecker::postWalkModuleDecl(ModuleDecl *D) { return true; }
