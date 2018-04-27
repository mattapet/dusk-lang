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
#include "dusk/Sema/Context.h"
#include "dusk/Sema/Scope.h"

using namespace dusk;
using namespace sema;


bool TypeChecker::preWalkBreakStmt(BreakStmt *S) {
  return false;
}

bool TypeChecker::preWalkReturnStmt(ReturnStmt *S) {
  return false;
}

bool TypeChecker::preWalkRangeStmt(RangeStmt *S) {
  return false;
}

bool TypeChecker::preWalkSubscriptStmt(SubscriptStmt *S) {
  return false;
}

bool TypeChecker::preWalkBlockStmt(BlockStmt *S) {
  Ctx.push();
  return false;
}

bool TypeChecker::preWalkExternStmt(ExternStmt *S) {
  return false;
}

bool TypeChecker::preWalkForStmt(ForStmt *S) {
  return false;
}

bool TypeChecker::preWalkFuncStmt(FuncStmt *S) {
  return false;
}

bool TypeChecker::preWalkIfStmt(IfStmt *S) {
  return false;
}

bool TypeChecker::preWalkWhileStmt(WhileStmt *S) {
  return false;
}


bool TypeChecker::postWalkBreakStmt(BreakStmt *S) {
  return true;
}

bool TypeChecker::postWalkReturnStmt(ReturnStmt *S) {
  return true;
}

bool TypeChecker::postWalkRangeStmt(RangeStmt *S) {
  if (!S->getStart()->getType()->isValueType()) {
    Diag.diagnose(S->getStart()->getLocStart(),
                  diag::expected_value_type_expression);
    return false;
  }
  if (!S->getEnd()->getType()->isValueType()) {
    Diag.diagnose(S->getEnd()->getLocStart(),
                  diag::expected_value_type_expression);
    return false;
  }
  return true;
}

bool TypeChecker::postWalkSubscriptStmt(SubscriptStmt *S) {
  llvm_unreachable("Not implemented yet.");
}

bool TypeChecker::postWalkBlockStmt(BlockStmt *S) {
  Ctx.pop();
  return true;
}

bool TypeChecker::postWalkExternStmt(ExternStmt *S) {
  return true;
}

bool TypeChecker::postWalkForStmt(ForStmt *S) {
  return true;
}

bool TypeChecker::postWalkFuncStmt(FuncStmt *S) {
  return true;
}

bool TypeChecker::postWalkIfStmt(IfStmt *S) {
  if (S->getCond()->getType()->isValueType())
    return true;
  Diag.diagnose(S->getCond()->getLocStart(),
                diag::expected_value_type_expression);
  return false;

}

bool TypeChecker::postWalkWhileStmt(WhileStmt *S) {
  if (S->getCond()->getType()->isValueType())
    return true;
  Diag.diagnose(S->getCond()->getLocStart(),
                diag::expected_value_type_expression);
  return false;
}

