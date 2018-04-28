//===--- Scope.h - Semantic scope -------------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_SEMA_SCOPE_H
#define DUSK_SEMA_SCOPE_H

namespace dusk {
class Stmt;
  
namespace sema {

class Scope {
public:
  enum ScopeFlag : unsigned {

    /// Scope corresponds to the scope of a function. It is valid for return
    /// statement to exist in this scope.
    FnScope = 0x01,

    /// This is \c while and \c for loop scope, where it is valid to use a \c
    /// break keyword.
    BreakScope = 0x02,

    /// Scope that corresponds to an arbitrary block of code.
    BlockScope = 0x04,

    /// Scope in \c if, \c while and \c for statements.
    ControlScope = 0x08,

    /// Scope correspoding to the function parameter delcaration.
    FnProtoScope = 0x010,
  };

private:
  /// A direct parent of current scope.
  Scope *Parent;

  /// Set of scope flags, which describe current scope.
  unsigned Flags;

  /// Depth of current scope.
  unsigned Depth = 0;

  /// Scope within a function definition.
  Scope *FnParent;

  /// Scope within another break scope.
  Scope *BreakParent;

  /// Scope within another control scope.
  Scope *ControlParent;

  /// Scope within another block scope.
  Scope *BlockParent;
  
  /// Statement representing current scope;
  Stmt *S;

public:
  Scope();
  Scope(Scope *Parent, unsigned ScopeFlags, Stmt *S = nullptr);

  /// Returns flags describing current scope.
  unsigned getFlags() const { return Flags; }

  /// Returns current scope depth.
  unsigned getDepth() const { return Depth; }

  /// Returns direct parent scope.
  Scope *getParent() const { return Parent; }

  /// Returns direct \c function parent scope.
  Scope *getFnParent() const { return FnParent; }

  /// Returns direct \c break parent scope.
  Scope *getBreakParent() const { return BreakParent; }

  /// Returns direct \c control parent scope.
  Scope *getControlParent() const { return ControlParent; }

  /// Returns direct \c block parent scope.
  Scope *getBlockParent() const { return BlockParent; }

  /// Returns \c true if this is a function scope, \c false otherwise.
  bool isFnScope() const { return Scope::FnScope & Flags; }

  /// Returns \c true if this is a break scope, \c false otherwise.
  bool isBreakScope() const { return Scope::BreakScope & Flags; }

  /// Returns \c true if this is a control scope, \c false otherwise.
  bool isControlScope() const { return Scope::ControlScope & Flags; }

  /// Returns \c true if this is a block scope, \c false otherwise.
  bool isBlockScope() const { return Scope::BlockScope & Flags; }
  
  /// Returns statement owning the current scope.
  Stmt *getStmt() const { return S; }
};

} // namespace sema

} // namespace dusk

#endif /* DUSK_SEMA_SCOPE_H */
