//===--- Scope.cpp --------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Sema/Scope.h"
#include "dusk/AST/Stmt.h"

using namespace dusk;
using namespace sema;

Scope::Scope()
    : Parent(nullptr), Flags(0), FnParent(nullptr), BreakParent(nullptr),
      ControlParent(nullptr), BlockParent(nullptr), S(nullptr) {}

Scope::Scope(Scope *P, unsigned SF, Stmt *S)
    : Parent(P), Flags(SF), Depth(P->Depth + 1), S(S) {
  FnParent = Parent->isFnScope() ? Parent : Parent->getFnParent();
  BlockParent = Parent->isBlockScope() ? Parent : Parent->getBlockParent();
  BreakParent = Parent->isBreakScope() ? Parent : Parent->getBreakParent();
  ControlParent =
      Parent->isControlScope() ? Parent : Parent->getControlParent();
}
