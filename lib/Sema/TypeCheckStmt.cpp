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

bool TypeChecker::preWalkBlockStmt(BlockStmt *S) {
  DeclCtx.push();
  if (auto Fn = dynamic_cast<FuncStmt *>(Scp.top().getStmt())) {
    auto Proto = static_cast<FuncDecl *>(Fn->getPrototype());
    auto Args = static_cast<VarPattern *>(Proto->getArgs());
    for (auto Arg : Args->getVars())
      if (!DeclCtx.declareLet(Arg)) {
        Diag.diagnose(Args->getLocStart(), diag::redefinition_of_identifier);
        return false;
      }
  }
  
  if (auto For = dynamic_cast<ForStmt *>(Scp.top().getStmt())) {
    if (!For->getRange()->walk(*this))
      return false;
    auto Rng = static_cast<RangeStmt *>(For->getRange());
    auto Iter = static_cast<ParamDecl *>(For->getIter());
    Iter->setType(Rng->getStart()->getType());
    DeclCtx.declareLet(Iter);
  }
  Scp.push(Scope(&Scp.top(), Scope::BlockScope, S));
  return true;
}

bool TypeChecker::preWalkExternStmt(ExternStmt *S) {
  Scp.push(Scope(&Scp.top(), Scope::FnScope, S));
  DeclCtx.push();
  return true;
}

bool TypeChecker::preWalkForStmt(ForStmt *S) {
  Scp.push(Scope(&Scp.top(), Scope::ControlScope | Scope::BreakScope, S));
  return true;
}

bool TypeChecker::preWalkFuncStmt(FuncStmt *S) {
  Scp.push(Scope(&Scp.top(), Scope::FnScope, S));
  return true;
}

bool TypeChecker::preWalkIfStmt(IfStmt *S) {
  Scp.push(Scope(&Scp.top(), Scope::ControlScope, S));
  return true;
}

bool TypeChecker::preWalkWhileStmt(WhileStmt *S) {
  Scp.push(Scope(&Scp.top(), Scope::ControlScope | Scope::BreakScope, S));
  return true;
}


bool TypeChecker::postWalkBreakStmt(BreakStmt *S) {
  if (Scp.top().isBreakScope() || Scp.top().getBreakParent() != nullptr)
    return true;
  Diag.diagnose(S->getLocStart(), diag::unexpected_break_stmt);
  return false;
}

bool TypeChecker::postWalkReturnStmt(ReturnStmt *S) {
  if (!Scp.top().isFnScope() && Scp.top().getFnParent() == nullptr) {
    Diag.diagnose(S->getLocStart(), diag::unexpected_return_stmt);
    return false;
  }
  auto &FnScp = Scp.top().isFnScope() ? Scp.top() : *Scp.top().getFnParent();
  
  auto F = static_cast<FuncStmt *>(FnScp.getStmt());
  auto FD = static_cast<FuncDecl *>(F->getPrototype());
  auto FTy = static_cast<FunctionType *>(FD->getType());
  
  if (!S->hasValue()) {
    if (FTy->getRetType()->isVoidType())
      return true;
    Diag.diagnose(S->getLocStart(), diag::return_missing_value);
    return false;
  }
  
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

