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
  std::vector<std::unique_ptr<ASTNode>> Nodes;
  std::vector<std::unique_ptr<Pattern>> Patterns;
  std::vector<std::unique_ptr<Type>> Types;
  std::vector<std::unique_ptr<TypeRepr>> TyReprs;
  
  bool IsError = false;
  
  ModuleDecl *RootModule;
  
public:
  ASTContext() = default;
  
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
  
  ArrayRef<std::unique_ptr<ASTNode>> getNodes() const { return Nodes; }
  
  ArrayRef<std::unique_ptr<Pattern>> getPatterns() const { return Patterns; }
  
  ArrayRef<std::unique_ptr<Type>> getTypes() const { return Types; }
  
  template <typename T> T *pushNode(std::unique_ptr<T> &&Node) {
    Nodes.push_back(std::move(Node));
    return static_cast<T *>(Nodes.back().get());
  }
  
  template <typename T> T *pushPattern(std::unique_ptr<T> &&Pattern) {
    Patterns.push_back(std::move(Pattern));
    return static_cast<T *>(Patterns.back().get());
  }
  
  template <typename T> T *pushType(std::unique_ptr<T> &&Type) {
    Types.push_back(std::move(Type));
    return static_cast<T *>(Patterns.back().get());
  }
  
  template <typename T> T *pushTypeRepr(std::unique_ptr<T> &&TyRepr) {
    TyReprs.push_back(std::move(TyRepr));
    return static_cast<T *>(TyRepr);
  }
};
  
} // namespace dusk

#endif /* DUSK_AST_CONTEXT_H */
