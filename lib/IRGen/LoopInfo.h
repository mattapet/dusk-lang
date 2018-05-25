//===--- LoopInfo.h - Loop information encapsulation ------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_GEN_CONTROL_H
#define DUSK_IRGEN_GEN_CONTROL_H

#include <stack>

namespace llvm {
  class BasicBlock;
}

namespace dusk {
class Stmt;
class IfStmt;
class ForStmt;
class WhileStmt;

namespace irgen {
class Context;

/// A simple class, than encapsulates information about generated loops.
class LoopInfo {
  /// Loop header block used e.g. for condition and/or initial variable
  /// allocations.
  llvm::BasicBlock *Header;

  /// Loop end block, used e.g. when break statement is encountered.
  llvm::BasicBlock *EndBlock;

public:
  /// Creates an invalid loop info.
  LoopInfo();

  /// Creates a valid loop info.
  LoopInfo(llvm::BasicBlock *H, llvm::BasicBlock *E);

  /// Returns \c true if loop info is valid, \c false otherwise.
  bool isValid() const { return Header != nullptr && EndBlock != nullptr; }

  /// Returns loop header block.
  llvm::BasicBlock *getHeader() const {
    assert(isValid() && "Accessing invalid loop info");
    return Header;
  }

  /// Returns loops end block.
  llvm::BasicBlock *getEndBlock() const {
    assert(isValid() && "Accessing invalid loop info");
    return EndBlock;
  }
};

/// Stack of loop information that are currently active.
class LoopInfoStack {
  /// Active loop information.
  std::stack<LoopInfo> Active;

public:
  LoopInfoStack() = default;

  /// Start a new loop.
  void push(llvm::BasicBlock *H, llvm::BasicBlock *E);

  /// End the top loop.
  void pop();

  /// Returns informatin about top level loop.
  const LoopInfo &getInfo() const { return Active.top(); }
};

/// A RAII object for changing loop info stack.
class LoopInfoRAII {
  LoopInfoStack &Stack;
  
public:
  LoopInfoRAII(LoopInfoStack &S, llvm::BasicBlock *H, llvm::BasicBlock *E);
  ~LoopInfoRAII();
};
  
} // namespace irgen
} // namesapce dusk

#endif /* DUSK_IRGEN_GEN_CONTROL_H */

