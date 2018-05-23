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

#include "TypeChecker.h"

using namespace dusk;
using namespace sema;

class StmtChecker : public StmtVisitor<StmtChecker> {
  typedef ASTVisitor super;
  friend super;

  TypeChecker &TC;

public:
  StmtChecker(TypeChecker &TC) : TC(TC) {}

private:
  void visitBreakStmt(BreakStmt *S) {
    if (!TC.ASTScope.isBreakScope())
      TC.diagnose(S->getLocStart(), diag::unexpected_break_stmt);
  }

  void visitReturnStmt(ReturnStmt *S) {
    // Return cannot occur outside of function scope
    if (!TC.ASTScope.isFnScope())
      TC.diagnose(S->getLocStart(), diag::unexpected_return_stmt);

    // Extract function type
    auto Fn = TC.ASTScope.getFnParent()->getStmt()->getFuncStmt();
    auto FnTy = Fn->getPrototype()->getType()->getFuncType();

    // Check if valid void type
    if (!S->hasValue()) {
      if (!FnTy->getRetType()->isVoidType())
        TC.diagnose(S->getLocStart(), diag::return_missing_value);
      return;
    }

    // Resolve return value and type
    auto E = TC.typeCheckExpr(S->getValue());
    TC.typeCheckEquals(FnTy->getRetType(), E->getType());
    S->setValue(E);
  }

  void visitSubscriptStmt(SubscriptStmt *S) {
    auto Val = TC.typeCheckExpr(S->getValue());
    S->setValue(Val);
  }

  void visitRangeStmt(RangeStmt *S) {
    auto Start = TC.typeCheckExpr(S->getStart());
    auto End = TC.typeCheckExpr(S->getStart());
    
    // Both range value must be of same type
    TC.typeCheckEquals(Start->getType(), End->getType());
    S->setStart(Start);
    S->setEnd(End);
  }

  void visitBlockStmt(BlockStmt *S) {
    PushScopeRAII Push(TC.ASTScope, Scope::BlockScope, S);
    for (auto &N : S->getNodes()) {
      if (auto D = dynamic_cast<Decl *>(N))
        TC.typeCheckDecl(D);

      else if (auto E = dynamic_cast<Expr *>(N))
        N = TC.typeCheckExpr(E);

      else if (auto S = dynamic_cast<Stmt *>(N))
        typeCheckStmt(S);

      else
        llvm_unreachable("Unexpected node type.");
    }
  }

  void visitExternStmt(ExternStmt *S) {
    PushScopeRAII Push(TC.ASTScope, Scope::FnScope, S);
    TC.typeCheckDecl(S->getPrototype());
  }

  void visitFuncStmt(FuncStmt *S) {
    PushScopeRAII Push(TC.ASTScope, Scope::FnScope, S);
    TC.typeCheckDecl(S->getPrototype());
    typeCheckStmt(S->getBody());
  }

  void visitForStmt(ForStmt *S) {
    PushScopeRAII Push(TC.ASTScope, Scope::BreakScope | Scope::ControlScope, S);
    TC.typeCheckDecl(S->getIter());
    typeCheckStmt(S->getRange());
    
    // Set iterator type BEFORE type checking the body.
    auto Ty = S->getRange()->getRangeStmt()->getStart()->getType();
    S->getIter()->setType(Ty);
    
    typeCheckStmt(S->getBody());
  }

  void visitWhileStmt(WhileStmt *S) {
    PushScopeRAII Push(TC.ASTScope, Scope::BreakScope | Scope::ControlScope, S);
    auto Cond = TC.typeCheckExpr(S->getCond());
    TC.typeCheckStmt(S->getBody());
    S->setCond(Cond);
  }

  void visitIfStmt(IfStmt *S) {
    PushScopeRAII Push(TC.ASTScope, Scope::ControlScope, S);
    auto Cond = TC.typeCheckExpr(S->getCond());
    typeCheckStmt(S->getThen());
    if (S->hasElseBlock())
      typeCheckStmt(S->getElse());
    S->setCond(Cond);
  }

public:
  void typeCheckStmt(Stmt *S) {
    super::visit(S);
  }
};

void TypeChecker::typeCheckStmt(Stmt *S) {
  StmtChecker(*this).typeCheckStmt(S);
}

// bool TypeChecker::preWalkBlockStmt(BlockStmt *S) {
//  Lookup.push();
//  if (auto Fn = dynamic_cast<FuncStmt *>(Scp.top().getStmt())) {
//    auto Proto = static_cast<FuncDecl *>(Fn->getPrototype());
//    auto Args = static_cast<VarPattern *>(Proto->getArgs());
//    for (auto Arg : Args->getVars())
//      if (!Lookup.declareVar(Arg)) {
//        diagnose(Args->getLocStart(), diag::redefinition_of_identifier);
//        return false;
//      }
//  }
//
//  if (auto For = dynamic_cast<ForStmt *>(Scp.top().getStmt())) {
//    if (!For->getRange()->walk(*this))
//      return false;
//    auto Rng = static_cast<RangeStmt *>(For->getRange());
//    auto Iter = static_cast<ParamDecl *>(For->getIter());
//    Iter->setType(Rng->getStart()->getType());
//    Lookup.declareLet(Iter);
//  }
//  Scp.push(Scope(&Scp.top(), Scope::BlockScope, S));
//  return true;
//}
//
// bool TypeChecker::preWalkExternStmt(ExternStmt *S) {
//  Scp.push(Scope(&Scp.top(), Scope::FnScope, S));
//  Lookup.push();
//  return true;
//}
//
// bool TypeChecker::preWalkForStmt(ForStmt *S) {
//  Scp.push(Scope(&Scp.top(), Scope::ControlScope | Scope::BreakScope, S));
//  return true;
//}
//
// bool TypeChecker::preWalkFuncStmt(FuncStmt *S) {
//  Scp.push(Scope(&Scp.top(), Scope::FnScope, S));
//  return true;
//}
//
// bool TypeChecker::preWalkIfStmt(IfStmt *S) {
//  Scp.push(Scope(&Scp.top(), Scope::ControlScope, S));
//  return true;
//}
//
// bool TypeChecker::preWalkWhileStmt(WhileStmt *S) {
//  Scp.push(Scope(&Scp.top(), Scope::ControlScope | Scope::BreakScope, S));
//  return true;
//}
//
// bool TypeChecker::postWalkBreakStmt(BreakStmt *S) {
//  if (Scp.top().isBreakScope() || Scp.top().getBreakParent() != nullptr)
//    return true;
//  diagnose(S->getLocStart(), diag::unexpected_break_stmt);
//  return false;
//}
//
// bool TypeChecker::postWalkReturnStmt(ReturnStmt *S) {
//  if (!Scp.top().isFnScope() && Scp.top().getFnParent() == nullptr) {
//    diagnose(S->getLocStart(), diag::unexpected_return_stmt);
//    return false;
//  }
//  auto &FnScp = Scp.top().isFnScope() ? Scp.top() : *Scp.top().getFnParent();
//
//  auto F = static_cast<FuncStmt *>(FnScp.getStmt());
//  auto FD = static_cast<FuncDecl *>(F->getPrototype());
//  auto FTy = static_cast<FunctionType *>(FD->getType());
//
//  if (!S->hasValue()) {
//    if (FTy->getRetType()->isVoidType())
//      return true;
//    diagnose(S->getLocStart(), diag::return_missing_value);
//    return false;
//  }
//
//  if (FTy->getRetType()->isClassOf(S->getValue()->getType()))
//    return true;
//
//  diagnose(S->getLocStart(), diag::type_missmatch);
//  return false;
//}
//
// bool TypeChecker::postWalkRangeStmt(RangeStmt *S) {
//  if (!S->getStart()->getType()->isValueType()) {
//    diagnose(S->getStart()->getLocStart(),
//             diag::expected_value_type_expression);
//    return false;
//  }
//  if (!S->getEnd()->getType()->isValueType()) {
//    diagnose(S->getEnd()->getLocStart(),
//    diag::expected_value_type_expression);
//    return false;
//  }
//  return true;
//}
//
// bool TypeChecker::postWalkSubscriptStmt(SubscriptStmt *S) {
//  return S->getValue()->getType()->isValueType();
//}
//
// bool TypeChecker::postWalkBlockStmt(BlockStmt *S) {
//  Lookup.pop();
//  Scp.pop();
//  return true;
//}
//
// bool TypeChecker::postWalkExternStmt(ExternStmt *S) {
//  Lookup.pop();
//  Scp.pop();
//  return true;
//}
//
// bool TypeChecker::postWalkForStmt(ForStmt *S) {
//  Scp.pop();
//  return true;
//}
//
// bool TypeChecker::postWalkFuncStmt(FuncStmt *S) {
//  Scp.pop();
//  return true;
//}
//
// bool TypeChecker::postWalkIfStmt(IfStmt *S) {
//  Scp.pop();
//  if (S->getCond()->getType()->isValueType())
//    return true;
//  diagnose(S->getCond()->getLocStart(), diag::expected_value_type_expression);
//  return false;
//}
//
// bool TypeChecker::postWalkWhileStmt(WhileStmt *S) {
//  Scp.pop();
//  if (S->getCond()->getType()->isValueType())
//    return true;
//  diagnose(S->getCond()->getLocStart(), diag::expected_value_type_expression);
//  return false;
//}
