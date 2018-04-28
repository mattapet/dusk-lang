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
  Scp.push(Scope(&Scp.top(), Scope::BlockScope, S));
  DeclCtx.push();
  return true;
}

bool TypeChecker::preWalkExternStmt(ExternStmt *S) {
  Scp.push(Scope(&Scp.top(), Scope::FnScope, S));
  DeclCtx.push();
  return true;
}

bool TypeChecker::preWalkForStmt(ForStmt *S) {
  Scp.push(Scope(&Scp.top(), Scope::ControlScope, S));
  return true;
}

bool TypeChecker::preWalkFuncStmt(FuncStmt *S) {
  Scp.push(Scope(&Scp.top(), Scope::FnScope, S));
  return true;
}

bool TypeChecker::preWalkIfStmt(IfStmt *S) {
  return false;
}

bool TypeChecker::preWalkWhileStmt(WhileStmt *S) {
  Scp.push(Scope(&Scp.top(), Scope::ControlScope, S));
  return false;
}


bool TypeChecker::postWalkBreakStmt(BreakStmt *S) {
  if (Scp.top().isBreakScope())
    return true;
  Diag.diagnose(S->getLocStart(), diag::unexpected_break_stmt);
  return false;
}

bool TypeChecker::postWalkReturnStmt(ReturnStmt *S) {
  if (!Scp.top().isFnScope()) {
    Diag.diagnose(S->getLocStart(), diag::unexpected_return_stmt);
    return false;
  }
  
  auto F = static_cast<FuncStmt *>(Scp.top().getFnParent()->getStmt());
  auto FD = static_cast<FuncDecl *>(F->getPrototype());
  auto FTy = static_cast<FunctionType *>(FD->getType());
  if (FTy->getRetType()->isClassOf(S->getValue()->getType()))
    return true;
  
  Diag.diagnose(S->getLocStart(), diag::type_missmatch);
  return false;
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
  DeclCtx.pop();
  Scp.pop();
  return true;
}

bool TypeChecker::postWalkExternStmt(ExternStmt *S) {
  DeclCtx.pop();
  Scp.pop();
  return true;
}

bool TypeChecker::postWalkForStmt(ForStmt *S) {
  Scp.pop();
  return true;
}

bool TypeChecker::postWalkFuncStmt(FuncStmt *S) {
  Scp.pop();
  return true;
}

bool TypeChecker::postWalkIfStmt(IfStmt *S) {
  Scp.pop();
  if (S->getCond()->getType()->isValueType())
    return true;
  Diag.diagnose(S->getCond()->getLocStart(),
                diag::expected_value_type_expression);
  return false;

}

bool TypeChecker::postWalkWhileStmt(WhileStmt *S) {
  Scp.pop();
  if (S->getCond()->getType()->isValueType())
    return true;
  Diag.diagnose(S->getCond()->getLocStart(),
                diag::expected_value_type_expression);
  return false;
}

