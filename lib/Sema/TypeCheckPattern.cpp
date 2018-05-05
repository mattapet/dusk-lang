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
#include "dusk/Sema/Sema.h"
#include "llvm/ADT/SmallVector.h"
#include <memory>

using namespace dusk;
using namespace sema;

bool TypeChecker::preWalkVarPattern(VarPattern *P) {
  return P->getType() == nullptr;
}

bool TypeChecker::preWalkExprPattern(ExprPattern *P) {
  return P->getType() == nullptr;
}

bool TypeChecker::postWalkVarPattern(VarPattern *P) {
  llvm::SmallVector<Type *, 128> Ty;
  for (auto V : P->getVars()) {
    if (!V->getType())
      return false;
    Ty.push_back(V->getType());
  }
  auto PTy = std::make_unique<PatternType>(std::move(Ty));
  P->setType(Ctx.pushType(std::move(PTy)));
  return true;
}

bool TypeChecker::postWalkExprPattern(ExprPattern *P) {
  llvm::SmallVector<Type *, 128> Ty;
  for (auto V : P->getValues()) {
    if (!V->getType())
      return false;
    Ty.push_back(V->getType());
  }
  auto PTy = std::make_unique<PatternType>(std::move(Ty));
  P->setType(Ctx.pushType(std::move(PTy)));
  return true;
}

