//===--- ASTVisitor.h - Dusk language AST visitor ---------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_AST_VISITOR_H
#define DUSK_AST_VISITOR_H

#include "dusk/AST/ASTNode.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/TypeRepr.h"

namespace dusk {

/// \brief This class represents a base visitor object of Dusk AST.
///
/// \description This class provides a default implementation of visit method
/// for \c ASTNode, \c Decl, \c Expr, \c Stmt and \c Pattern. Derived classes
/// may add another "base type" methods, but should never override and/or change
/// their default behavior.
///
/// Class also provides a default implementation for all concrete Dusk AST
/// nodes. Derived classes should modify them to implement their own traversal
/// behaviour.
///
/// All of the \c visit methods return a \c bool indicating validity of
/// traversal. \c true indicates valid traversal of given node/subtree and
/// visitor may continue traversing. \c false means failure, upon which visitor
/// must immedietly terminate traversal of the AST.
template <typename Derived, typename DeclRetTy = void,
          typename ExprRetTy = void, typename StmtRetTy = void,
          typename PatternRetTy = void, typename TypeReprRetTy = void>
class ASTVisitor {
public:
  /// Returns a reference to the derived class.
  Derived &getDerived() { return *static_cast<Derived *>(this); }

  /// Visit a concrete declaration node.
  DeclRetTy visit(Decl *D) {
    switch (D->getKind()) {
#define DECL(CLASS, PARENT)                                                    \
  case DeclKind::CLASS:                                                        \
    return getDerived().visit##CLASS##Decl(static_cast<CLASS##Decl *>(D));

#include "dusk/AST/DeclNodes.def"
    }
    llvm_unreachable("All cases handeled");
  }

  /// Visit a concrete expression node.
  ExprRetTy visit(Expr *E) {
    switch (E->getKind()) {
#define EXPR(CLASS, PARENT)                                                    \
  case ExprKind::CLASS:                                                        \
    return getDerived().visit##CLASS##Expr(static_cast<CLASS##Expr *>(E));

#include "dusk/AST/ExprNodes.def"
    }
    llvm_unreachable("All cases handeled");
  }

  /// Visit a concrete statement node.
  StmtRetTy visit(Stmt *S) {
    switch (S->getKind()) {
#define STMT(CLASS, PARENT)                                                    \
  case StmtKind::CLASS:                                                        \
    return getDerived().visit##CLASS##Stmt(static_cast<CLASS##Stmt *>(S));

#include "dusk/AST/StmtNodes.def"
    }
    llvm_unreachable("All cases handeled.");
  }

  /// Visit a concrete pattern node.
  PatternRetTy visit(Pattern *P) {
    switch (P->getKind()) {
#define PATTERN(CLASS, PARENT)                                                 \
  case PatternKind::CLASS:                                                     \
    return getDerived().visit##CLASS##Pattern(static_cast<CLASS##Pattern *>(P));

#include "dusk/AST/PatternNodes.def"
    }
    llvm_unreachable("All cases handeles");
  }

  /// Visits concrete TypeRepr
  TypeReprRetTy visit(TypeRepr *T) {
    switch (T->getKind()) {
#define TYPE_REPR(CLASS, PARENT)                                               \
  case TypeReprKind::CLASS:                                                    \
    return getDerived().visit##CLASS##TypeRepr(                                \
        static_cast<CLASS##TypeRepr *>(T));

#include "dusk/AST/TypeReprNodes.def"
    }
    llvm_unreachable("All cases handeled.");
  }
};

template <typename Derived, typename DeclRetTy = void>
using DeclVisitor = ASTVisitor<Derived, DeclRetTy, void, void, void, void>;

template <typename Derived, typename ExprRetTy = void>
using ExprVisitor = ASTVisitor<Derived, void, ExprRetTy, void, void, void>;

template <typename Derived, typename StmtRetTy = void>
using StmtVisitor = ASTVisitor<Derived, void, void, StmtRetTy, void, void>;

template <typename Derived, typename PatternRetTy = void>
using PatternVisitor =
    ASTVisitor<Derived, void, void, void, PatternRetTy, void>;

template <typename Derived, typename TypeReprRetTy = void>
using TypeReprVisitor =
    ASTVisitor<Derived, void, void, void, void, TypeReprRetTy>;

} // namespace dusk

#endif /* DUSK_AST_VISITOR_H */
