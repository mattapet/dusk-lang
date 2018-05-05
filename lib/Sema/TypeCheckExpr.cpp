//===--- TypeChecker.cpp --------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "TypeChecker.h"

#include "dusk/AST/Diagnostics.h"
#include "dusk/AST/NameLookup.h"

using namespace dusk;
using namespace sema;

bool TypeChecker::postWalkNumberLiteralExpr(NumberLiteralExpr *E) {
  return true;
}

bool TypeChecker::postWalkIdentifierExpr(IdentifierExpr *E) {
  // Check if it's a value type
  if (auto D = DeclCtx.getVal(E->getName())) {
    E->setType(D->getType());
    return true;
  }
  // Check if it's a function reference
  if (auto Fn = DeclCtx.getFunc(E->getName())) {
    E->setType(Fn->getType());
    return true;
  }
  diagnose(E->getLocStart(), diag::undefined_identifier);
  return false;
}

bool TypeChecker::postWalkParenExpr(ParenExpr *E) {
  E->setType(E->getExpr()->getType());
  return true;
}

bool TypeChecker::postWalkAssignExpr(AssignExpr *E) {
  auto Ident = dynamic_cast<IdentifierExpr *>(E->getDest());
  // Check if its an assignable expression
  if (!Ident || DeclCtx.getFunc(Ident->getName())) {
    diagnose(E->getDest()->getLocStart(), diag::expression_not_assignable);
    return false;
  }
  // Check if
  if (DeclCtx.getVal(Ident->getName()) && !DeclCtx.getVar(Ident->getName())) {
    diagnose(E->getDest()->getLocStart(), diag::cannot_reassign_let_value);
  }

  // Check type match.
  if (E->getDest()->getType()->isClassOf(E->getSource()->getType())) {
    E->setType(E->getDest()->getType());
    return true;
  }
  diagnose(E->getDest()->getLocEnd(), diag::type_missmatch);
  return false;
}

bool TypeChecker::postWalkInfixExpr(InfixExpr *E) {
  if (!E->getLHS()->getType() || !E->getRHS()->getType())
    return false;

  // Check type match.
  if (E->getLHS()->getType()->isClassOf(E->getRHS()->getType())) {
    E->setType(E->getLHS()->getType());
    return true;
  }
  diagnose(E->getOp().getLoc(), diag::type_missmatch);
  return false;
}

bool TypeChecker::postWalkPrefixExpr(PrefixExpr *E) {
  E->setType(E->getDest()->getType());
  return true;
}

bool TypeChecker::postWalkCallExpr(CallExpr *E) {
  auto FTy = dynamic_cast<FunctionType *>(E->getCalle()->getType());

  // Check if references a function
  if (!FTy) {
    diagnose(E->getCalle()->getLocStart(), diag::func_call_non_func_type);
    return false;
  }

  // Check is arguments are the same as in proto
  if (E->getArgs()->getType()->isClassOf(FTy->getArgsType())) {
    E->setType(FTy->getRetType());
    return true;
  } else {
    diagnose(E->getArgs()->getLocStart(), diag::arguments_mismatch);
    return false;
  }
}

bool TypeChecker::postWalkSubscriptExpr(SubscriptExpr *E) {
  llvm_unreachable("Not implemented.");
}
