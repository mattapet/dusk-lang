//===--- ASTContext.h - Object holding AST state ----------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_AST_CONTEXT_H
#define DUSK_AST_CONTEXT_H

#include "dusk/Basic/LLVM.h"
#include "dusk/AST/ASTNode.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/Type.h"
#include "dusk/AST/TypeRepr.h"
#include "llvm/ADT/ArrayRef.h"
#include <memory>
#include <vector>
#include <functional>

namespace dusk {
class Decl;
class ModuleDecl;
class Expr;
class Stmt;
class ASTNode;
class Pattern;
class Type;
class VoidType;
class IntType;
class TypeRepr;

/// This class owns all of the nodes, which are part of the AST.
class ASTContext {
  std::vector<std::function<void(void)>> Cleanups;

  bool IsError = false;

  ModuleDecl *RootModule;

public:
  ASTContext();
  ~ASTContext();

  /// Returns pointer to the root module of the AST.
  ModuleDecl *getRootModule() const { return RootModule; }

  /// Sets the root module of the AST.
  void setRootModule(ModuleDecl *RM) { RootModule = RM; }

  /// Returns \c true if \c setError has been called upon the context, \c false
  /// otherwise.
  bool isError() const { return IsError; }

  /// Sets AST error state to \c true.
  ///
  /// \node This action is ireversible.
  void setError() { IsError = true; }

  /// Allocates a given number of bytes.
  ///
  /// All memory allocated by a \c ASTCotext instance will be freed during
  /// the destruction of the instance.
  void *Allocate(size_t Bytes);

private:
  // MARK: - Type singletons
  IntType *TheIntType;
  VoidType *TheVoidType;

public:
  IntType *getIntType() const;
  VoidType *getVoidType() const;

private:
  ASTContext(const ASTContext &) = delete;
  ASTContext &operator=(const ASTContext &) = delete;
};

} // namespace dusk

#endif /* DUSK_AST_CONTEXT_H */
