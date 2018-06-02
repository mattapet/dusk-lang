//===--- ASTNode.h - Dusk language AST node ---------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_AST_NODE_H
#define DUSK_AST_NODE_H

#include "dusk/Basic/LLVM.h"
#include "llvm/Support/SMLoc.h"
#include <cstdint>

namespace dusk {
class ASTContext;
class ASTWalker;

/// \brief Abstract base class for all derived classes, that wish to be used to
///  construct the \c AST.
///
/// \description ASTNode also declares a unified interface that us used
///  by objects operating on the AST, such as instances of \c ASTWalker.
///  Therefore, to preserve valid behavior of AST all derived classes must
///  implement this interface correctly.
class ASTNode {
  ASTNode(const ASTNode &other) = delete;
  ASTNode &operator=(const ASTNode &other) = delete;

public:
  /// Destructs a basic AST node.
  ASTNode() = default;

  /// Returns text range in source file represented by the node.
  virtual SMRange getSourceRange() const = 0;

  /// Returns start of the text range represented by the node.
  SMLoc getLocStart() const { return getSourceRange().Start; }

  /// Returns end of the text range represented by the node.
  SMLoc getLocEnd() const { return getSourceRange().End; }

  /// Walks AST node.
  ///
  /// \return \c true if the node was walked properly and may continue
  ///  traversing the AST, \c false if should terminate.
  bool walk(ASTWalker &Walker);

public:
  /// Only allow allocation using \c ASTContext
  void *operator new(std::size_t Bytes, ASTContext &Context);
  void *operator new(std::size_t Bytes, void *Mem) throw() { return Mem; }
};

} // namespace dusk

#endif /* DUSK_AST_NODE_H */
