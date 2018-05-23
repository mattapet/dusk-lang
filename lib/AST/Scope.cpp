//===--- Scope.cpp --------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/Scope.h"
#include "dusk/AST/Stmt.h"

using namespace dusk;

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

// MARK: - Scope change

PushScopeRAII::PushScopeRAII(Scope &Scp, unsigned Flags, Stmt *S)
    : Popped(false), Parent(Scp), Self(Scp) {
  Self = Scope(&Parent, Flags, S);
}

PushScopeRAII::~PushScopeRAII() {
  if (!Popped)
    Self = Parent;
}

void PushScopeRAII::pop() {
  assert(isValid() && "Popping already popped scope.");
  Self = Parent;
  Popped = true;
}
