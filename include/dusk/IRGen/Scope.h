//===--- Scope.h - Dusck scope interface ------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//
//
// This file implements Dusk scope interaface.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_SCOPE_H
#define DUSK_SCOPE_H

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Value.h"

namespace dusk {
  
namespace irgen {

enum struct EntryKind {
  Var,
  Const,
  Func
};
  
struct ScopeEntry {
  EntryKind Kind;
  llvm::StringRef ID;
  llvm::Value *Value;
};
  
/// RAII object for changing the scope.
class ScopeChange {
  
};
  
/// Creates a linked list of storage objects, each representing a single layer
/// of declarations. If an entry is not found withing a given scope storage,
/// the call is forwared to its parent.
class ScopeStorage {
  ScopeStorage *Parent;
  llvm::StringMap<ScopeEntry> Entries;
  friend class ScopeStorage;
  
public:
  ScopeEntry *get(llvm::StringRef Str);
};
  
class Scope {
  friend class ScopeStorage;
  ScopeStorage *Storage;
  unsigned Depth;
  
public:
  ScopeEntry *getEntry(llvm::StringRef Str);
  
};
  
} // namespace irgen
  
} // namespace dusk

#endif /* DUSK_SCOPE_H */
