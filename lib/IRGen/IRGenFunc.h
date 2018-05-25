//===--- IRGenFunc.h - IR generation state for functions --------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_IRGEN_FUNC_H
#define DUSK_IRGEN_IRGEN_FUNC_H

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"

#include "Address.h"
#include "IRGenModule.h"
#include "LoopInfo.h"

#include <stack>

namespace llvm {
class StringRef;
class Function;
class BasicBlock;
class Value;
}

namespace dusk {
class Decl;
class FuncDecl;
class FuncStmt;

namespace irgen {

class IRGenFunc {
public:
  /// Module which part is this function of.
  IRGenModule &IRGM;

  /// Current IR builder
  llvm::IRBuilder<> &Builder;

  /// Actual function.
  llvm::Function *Fn;

  /// Function statement.
  FuncDecl *Proto;

  /// Loop stack encapsulation.
  LoopInfoStack LoopStack;

  IRGenFunc(IRGenModule &IRGM, llvm::IRBuilder<> &B, llvm::Function *Fn,
            FuncStmt *F);

  ~IRGenFunc();

  friend class Scope;

private:
  /// A block, where all function initialization happen. E.g. parameter
  /// allocation and initialization.
  llvm::BasicBlock *HeaderBlock;

  /// Main function body block.
  llvm::BasicBlock *BodyBlock;

  /// A common return block.
  llvm::BasicBlock *RetBlock;

  /// A temporary alloca, that holds a return value.
  ///
  /// \node This address is invalid if the function does not return a value.
  Address RetValue;

public:
  /// Returns function's header block
  llvm::BasicBlock *getHeaderBlock() const { return HeaderBlock; }
  /// Returns function's body block
  llvm::BasicBlock *getBodyBlock() const { return BodyBlock; }
  /// Returns function's return block
  llvm::BasicBlock *getRetBlock() const { return RetBlock; }

  void setRetVal(llvm::Value *V);

  Address declare(Decl *N);

  Address getVal(StringRef N);

  llvm::Function *getFunc(StringRef N);

private:
  /// Emits function header block.
  void emitHeader();

  /// Return function return block
  void emitRet();
};

} // namespace irgen
} // namespace dusk

#endif /* DUSK_IRGEN_IRGEN_FUNC_H */

