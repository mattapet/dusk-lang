//===--- Scope.cpp - Dusck scope implementation ---------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//
//
// This file implements Dusk scope implementation.
//
//===----------------------------------------------------------------------===//

#include "dusk/IRGen/Scope.h"

using namespace dusk;
using namespace irgen;

ScopeEntry *ScopeStorage::get(llvm::StringRef Str) {
  if (Entries.find(Str) != Entries.end())
    return &Entries[Str];
  if (Parent)
    return Parent->get(Str);
  return nullptr;
}

ScopeEntry *Scope::getEntry(llvm::StringRef Str) {
  return Storage->get(Str);
}

