//===--- Context.h - Semantic declaration context ---------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_SEMA_CONTEXT_H
#define DUSK_SEMA_CONTEXT_H

#include "dusk/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringMap.h"
#include <memory>

namespace dusk {
class Decl;
  
namespace sema {

/// Represents current context declaration level.
///
/// Constructors of this struct are private, instances of \c ContextVals class
/// can be made by only \c Context instances.
struct ContextImpl {
  /// Holds pointer to the parent context.
  std::unique_ptr<ContextImpl> Parent = nullptr;
  
  /// Holds constant declarations of current scope.
  llvm::StringMap<Decl *> Consts;
  
  /// Holds variable declarations of current scope.
  llvm::StringMap<Decl *> Vars;
  
public:
  /// Returns \c true if there is a reachable value delcaration for given name
  /// the scope, \c false otherwise.
  bool contains(StringRef Str) const { return get(Str) != nullptr; }
  
  /// Returns \c true if a declaration for given name was performed in current
  /// scope.
  bool isDeclared(StringRef Str) const;
  
  /// Returns variable for given name, if found, \c nullptr otherwise.
  Decl *getVar(StringRef Str) const;
  
  /// Returns variable for given name, if found, \c nullptr otherwise.
  Decl *getVar(StringRef Str);
  
  /// Returns variable for given name, if found, \c nullptr otherwise.
  Decl *get(StringRef Str) const;
  
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
  llvm::StringMap<Decl *> Funcs;
  ContextImpl *Impl;
  unsigned Depth = 0;
  
public:
  Context();
  ~Context();
  
  /// Returns current depth of the context.
  unsigned getDepth() const { return Depth; }
  
  /// \brief Declares a variable in current scope.
  ///
  /// \return \c true on success, \c false if the current scope is already
  /// a variable, constatnt or function with the same identifier.
  bool declareVar(Decl *);
  
  /// \brief Declares a param in current scope.
  ///
  /// \return \c true on success, \c false if the current scope is already
  /// a variable, constatnt or function with the same identifier.
  bool declareLet(Decl *);
  
  /// \brief Declares a function.
  ///
  /// \note Function can be delcared only in the global scope.
  ///
  /// \return \c true on success, \c false if a function with given identifier
  /// already exists.
  bool declareFunc(Decl *);
  
  /// Returns a value for given identifier. Can be both, reference variable
  /// and constant.
  ///
  /// If no value is found, \c nullptr is returned.
  Decl *getVal(StringRef Str) const;
  
  /// Returns a value for given identifier. Can be both, reference variable
  /// and constant.
  ///
  /// If no value is found, \c nullptr is returned.
  Decl *getVar(StringRef Str) const;
  
  /// Returns function type for given identifier.
  ///
  /// If no type is found, \c nullptr is returned.
  Decl *getFunc(StringRef Str);
  
  /// Returns \c true, if in the current scope if a declaration associated with
  /// given identifier.
  bool contains(StringRef Str) const;
  
  /// Pushes a new scope to the internal stack.
  void push();
  
  /// Pops current scope from the internal stack.
  void pop();
};

} // namespace sema
  
} // namespace dusk

#endif /* DUSK_SEMA_CONTEXT_H */
