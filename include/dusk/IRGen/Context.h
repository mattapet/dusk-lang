//===--- Context.h - Dusck context interface --------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//
//
// This file implements Dusk context interaface.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_CONTEXT_H
#define DUSK_IRGEN_CONTEXT_H

#include "dusk/Basic/LLVM.h"
#include "dusk/AST/Scope.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/BasicBlock.h"
#include <utility>
#include <memory>

namespace dusk {
class Decl;
class FuncDecl;
class ParamDecl;

namespace irgen {

/// Represents a basic block range.
struct BlockRange {
  llvm::BasicBlock *Start;
  llvm::BasicBlock *End;

  BlockRange();
  BlockRange(llvm::BasicBlock *S, llvm::BasicBlock *E);
};

/// Represents current context declaration level.
///
/// Constructors of this struct are private, instances of \c ContextVals class
/// can be made by only \c Context instances.
struct ContextImpl {
  /// Holds pointer to the parent context.
  std::unique_ptr<ContextImpl> Parent = nullptr;

  /// Holds value declarations of current scope.
  llvm::StringMap<llvm::Value *> Values;

public:
  /// Returns \c true if there is a reachable value delcaration for given name
  /// the scope, \c false otherwise.
  bool contains(StringRef Str) const { return get(Str) != nullptr; }

  /// Returns \c true if a declaration for given name was performed in current
  /// scope.
  bool isDeclared(StringRef Str) const;

  /// Returns variable for given name, if found, \c nullptr otherwise.
  llvm::Value *get(StringRef Str) const;

  /// Pushes a new context to the stack and returns a pointer to the top of the
  /// stack.
  ContextImpl *push();

  /// Pop a top context from the stack and returns a pointer to the top of the
  /// stack.
  ///
  /// \note This method calls \c delete \c this on itself, therefore noone
  /// should access this object after calling this method.
  ContextImpl *pop();

private:
  friend class Context;
  ContextImpl() = default;
  ContextImpl(ContextImpl *P);

  ContextImpl(const ContextImpl &other) = delete;
  ContextImpl &operator=(const ContextImpl &other) = delete;
};

/// Represents a current declaration context.
///
/// Holds declaration of variables, constatnts and functions.
class Context {
  typedef std::pair<llvm::BasicBlock *, llvm::BasicBlock> BBRange;

  llvm::LLVMContext &Ctx;
  llvm::StringMap<llvm::FunctionType *> Funcs;
  llvm::DenseMap<Scope *, BlockRange> ScopeRanges;
  ContextImpl *Impl;
  unsigned Depth = 0;

public:
  llvm::Module *Module;
  llvm::IRBuilder<> &Builder;

  Context(llvm::LLVMContext &C, llvm::Module *M, llvm::IRBuilder<> &B);
  ~Context();

  operator llvm::LLVMContext &() { return Ctx; }

  /// Returns current depth of the context.
  unsigned getDepth() const { return Depth; }

  /// \brief Declares a value in current scope.
  ///
  /// \return \c true on success, \c false if the current scope is already
  /// a value or function with the same identifier.
  bool declareVal(const Decl *);
  
  /// \brief Declares a value in current scope.
  ///
  /// \return \c true on success, \c false if the current scope is already
  /// a value or function with the same identifier.
  bool declareVal(const Decl *, llvm::Function *);

  /// \brief Declares a function.
  ///
  /// \note Function can be delcared only in the global scope.
  ///
  /// \return \c true on success, \c false if a function with given identifier
  /// already exists.
  bool declareFunc(const FuncDecl *);

  /// Returns a value for given identifier.
  ///
  /// If no value is found, \c nullptr is returned.
  llvm::Value *getVal(StringRef Str) const;


  /// Returns function type for given identifier.
  ///
  /// If no type is found, \c nullptr is returned.
  llvm::FunctionType *getFuncProto(StringRef Str);

  /// Return function for given identifier.
  llvm::Function *getFunc(StringRef Str);

  /// Returns \c true, if in the current scope if a declaration associated with
  /// given identifier.
  bool contains(StringRef Str) const;

  /// Pushes a new scope to the internal stack.
  void push();

  /// Pops current scope from the internal stack.
  void pop();

  void pushScope(Scope *S, BlockRange R);

  BlockRange &getRange(Scope *S);

  void popScope(Scope *S);
};

} // namespace irgen

} // namespace dusk

#endif /* DUSK_IRGEN_CONTEXT_H */
