#include "TypeChecker.h"
#include "dusk/AST/Diagnostics.h"
#include "dusk/Sema/Context.h"

using namespace dusk;
using namespace sema;

bool TypeChecker::preWalkNumberLiteralExpr(NumberLiteralExpr *E) {
  return false;
}

bool TypeChecker::preWalkIdentifierExpr(IdentifierExpr *E) {
  return E->getType() == nullptr;
}

bool TypeChecker::preWalkParenExpr(ParenExpr *E) {
  return E->getType() == nullptr;
}

bool TypeChecker::preWalkAssignExpr(AssignExpr *E) {
  return E->getType() == nullptr;
}

bool TypeChecker::preWalkInfixExpr(InfixExpr *E) {
  return E->getType() == nullptr;
}

bool TypeChecker::preWalkPrefixExpr(PrefixExpr *E) {
  return E->getType() == nullptr;
}

bool TypeChecker::preWalkCallExpr(CallExpr *E) {
  return E->getType() == nullptr;
}

bool TypeChecker::preWalkSubscriptExpr(SubscriptExpr *E) { return false; }

bool TypeChecker::postWalkNumberLiteralExpr(NumberLiteralExpr *E) {
  llvm_unreachable("Not implemented");
}

bool TypeChecker::postWalkIdentifierExpr(IdentifierExpr *E) {
  // Check if it's a value type
  if (auto D = Ctx.getVal(E->getName())) {
    E->setType(D->getType());
    return true;
  }
  // Check if it's a function reference
  if (auto Fn = Ctx.getFunc(E->getName())) {
    E->setType(Fn->getType());
    return true;
  }
  Diag.diagnose(E->getLocStart(), diag::undefined_identifier);
  return false;
}

bool TypeChecker::postWalkParenExpr(ParenExpr *E) {
  E->setType(E->getExpr()->getType());
  return true;
}

bool TypeChecker::postWalkAssignExpr(AssignExpr *E) {
  // Check type match.
  if (E->getDest()->getType()->isClassOf(E->getSource()->getType())) {
    E->setType(E->getDest()->getType());
    return true;
  }
  Diag.diagnose(E->getDest()->getLocEnd(), diag::type_missmatch);
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
  Diag.diagnose(E->getOp().getLoc(), diag::type_missmatch);
  return false;
}

bool TypeChecker::postWalkPrefixExpr(PrefixExpr *E) {
  E->setType(E->getDest()->getType());
  return true;
}

bool TypeChecker::postWalkCallExpr(CallExpr *E) {
  auto FTy = static_cast<FunctionType *>(E->getCalle()->getType());
  E->setType(FTy->getRetType());
  
  if (E->getArgs()->getType()->isClassOf(E->getArgs()->getType())) {
    E->setType(FTy->getRetType());
    return true;
  } else {
    return false;
  }
}

bool TypeChecker::postWalkSubscriptExpr(SubscriptExpr *E) {
  llvm_unreachable("Not implemented.");
}
