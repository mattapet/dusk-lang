//===--- ASTWalker.h - Class for walking the AST ---------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_AST_WALKER_H
#define DUSK_AST_WALKER_H

#include "dusk/Basic/LLVM.h"
#include <utility>

namespace dusk {
class Decl;
class Expr;
class Stmt;
class Pattern;
class TypeRepr;

/// \brief Base class for all classes, that whish to traverse the AST.
///
/// \description \c ASTWalker provides default API for walking the AST. The API
///  consists of \c postWalk and \c preWalk methods called by traversed
///  AST nodes.
class ASTWalker {
public:
  ASTWalker() = default;
  virtual ~ASTWalker() = default;

  /// This method is called before a declaration is being walked.
  ///
  /// \param D A declaration node that will be walked.
  ///
  /// \return \c true if the node should be walked, \c false otherwise.
  ///
  /// Default implementation returns \c true.
  virtual bool preWalkDecl(Decl *D) { return true; }

  /// This method is called after declaration children were walked.
  ///
  /// \param D A declaration node that was walked.
  ///
  /// \return \c true if the node should continue traversal,
  ///  \c false otherwise.
  ///
  /// Default implementation returns \c true.
  virtual bool postWalkDecl(Decl *D) { return true; }

  /// This method is called before an expression is being walked.
  ///
  /// \param E An expression node that will be walked.
  ///
  /// \return A pair consisting of a \c bool indicating, whether this node
  ///  should be skipped or not and an expression that should replace this
  ///  expression in the tree. If \c null is returned, the traversal is
  ///  terminated.
  ///
  /// Default implementation returns \c {true, E}.
  virtual std::pair<bool, Expr *> preWalkExpr(Expr *E) { return {true, E}; }

  /// This method is called after expression children were walked.
  ///
  /// \param E An expression node that was walked.
  ///
  /// \return \c null, in which case the walk is terminater, otherwise the
  ///  returned expression replaces walked expression in the tree.
  ///
  /// Default implementation returns its argument.
  virtual Expr *postWalkExpr(Expr *E) { return E; }

  /// This method is called before a statement is being walked.
  ///
  /// \param S A statement node that will be walked.
  ///
  /// \return \c true if the node should be walked, \c false otherwise.
  ///
  /// Default implementation returns \c true.
  virtual bool preWalkStmt(Stmt *S) { return true; }

  /// This method is called after statement children were walked.
  ///
  /// \param S A statement node that was walked.
  ///
  /// \return \c true if the node should continue traversal,
  ///  \c false otherwise.
  ///
  /// Default implementation returns \c true.
  virtual bool postWalkStmt(Stmt *S) { return true; }

  /// This method is called before a pattern is being walked.
  ///
  /// \param P A pattern node that will be walked.
  ///
  /// \return \c true if the node should be walked, \c false otherwise.
  ///
  /// Default implementation returns \c true.
  virtual bool preWalkPattern(Pattern *P) { return true; }

  /// This method is called after pattern children were walked.
  ///
  /// \param P A pattern node that was walked.
  ///
  /// \return \c true if the node should continue traversal,
  ///  \c false otherwise.
  ///
  /// Default implementation returns \c true.
  virtual bool postWalkPattern(Pattern *P) { return true; }

  /// This method is called before a type represenation is being walked.
  ///
  /// \param Tr A type representation node that will be walked.
  ///
  /// \return \c true if the node should be walked, \c false otherwise.
  ///
  /// Default implementation returns \c true.
  virtual bool preWalkTypeRepr(TypeRepr *Tr) { return true; }

  /// This method is called after type representation children were walked.
  ///
  /// \param Tr A type representation node that was walked.
  ///
  /// \return \c true if the node should continue traversal,
  ///  \c false otherwise.
  ///
  /// Default implementation returns \c true.
  virtual bool postWalkTypeRepr(TypeRepr *Tr) { return true; }

private:
  ASTWalker(const ASTWalker &) = delete;
  ASTWalker &operator=(const ASTWalker &) = delete;
};

} // namespace dusk

#endif /* DUSK_AST_WALKER_H */
