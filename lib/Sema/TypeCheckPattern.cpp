//===--- TypeCheckPattern.cpp ---------------------------------------------===//
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
#include "llvm/ADT/SmallVector.h"
#include <memory>

using namespace dusk;
using namespace sema;

namespace {

/// Pattern type checker.
class PatternChecker : public PatternVisitor<PatternChecker> {
  TypeChecker &TC;

  typedef ASTVisitor super;

  friend super;

public:
  PatternChecker(TypeChecker &TC) : TC(TC) {}

private:
  void visitExprPattern(ExprPattern *P) {
    llvm::SmallVector<Type *, 128> Tys;
    for (auto &E : P->getValues()) {
      E = TC.typeCheckExpr(E);
      Tys.push_back(E->getType());
      if (!Tys.back())
        return;
    }
    P->setType(new (TC.Ctx) PatternType(std::move(Tys)));
  }

  void visitVarPattern(VarPattern *P) {
    llvm::SmallVector<Type *, 128> Tys;
    for (auto D : P->getVars()) {
      TC.typeCheckDecl(D);
      Tys.push_back(D->getType());
      if (!Tys.back())
        return;
    }
    P->setType(new (TC.Ctx) PatternType(std::move(Tys)));
  }

public:
  void typeCheckPattern(Pattern *P) { super::visit(P); }
};

} // anonymous namespace

void TypeChecker::typeCheckPattern(Pattern *P) {
  PatternChecker(*this).typeCheckPattern(P);
}

// bool TypeChecker::preWalkVarPattern(VarPattern *P) {
//  return P->getType() == nullptr;
//}
//
// bool TypeChecker::preWalkExprPattern(ExprPattern *P) {
//  return P->getType() == nullptr;
//}
//
// bool TypeChecker::postWalkVarPattern(VarPattern *P) {
//  llvm::SmallVector<Type *, 128> Ty;
//  for (auto V : P->getVars()) {
//    if (!V->getType())
//      return false;
//    Ty.push_back(V->getType());
//  }
//
//  P->setType(new(Ctx) PatternType(std::move(Ty)));
//  return true;
//}
//
// bool TypeChecker::postWalkExprPattern(ExprPattern *P) {
//  llvm::SmallVector<Type *, 128> Ty;
//  for (auto V : P->getValues()) {
//    if (!V->getType())
//      return false;
//    Ty.push_back(V->getType());
//  }
//
//  P->setType(new(Ctx) PatternType(std::move(Ty)));
//  return true;
//}
//

