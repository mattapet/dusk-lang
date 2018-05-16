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
template <typename Derived> class ASTVisitor {
public:
  /// Returns a reference to the derived class.
  Derived &getDerived() { return *static_cast<Derived *>(this); }

  bool visit_(ASTNode *N) {
    if (auto *D = dynamic_cast<Decl *>(N))
      return visit_(D);
    if (auto *E = dynamic_cast<Expr *>(N))
      return visit_(E);
    if (auto *S = dynamic_cast<Stmt *>(N))
      return visit_(S);

    llvm_unreachable("Unexpected node");
  }

  /// Visit a concrete declaration node.
  bool visit_(Decl *D) {
    switch (D->getKind()) {
    case DeclKind::Let:
      return getDerived().visit(static_cast<LetDecl *>(D));
    case DeclKind::Func:
      return getDerived().visit(static_cast<FuncDecl *>(D));
    case DeclKind::Module:
      return getDerived().visit(static_cast<ModuleDecl *>(D));
    case DeclKind::Param:
      return getDerived().visit(static_cast<ParamDecl *>(D));
    case DeclKind::Var:
      return getDerived().visit(static_cast<VarDecl *>(D));
    }
  }

  /// Visit a conrete expression node.
  bool visit_(Expr *E) {
    switch (E->getKind()) {
    case ExprKind::NumberLiteral:
      return getDerived().visit(static_cast<NumberLiteralExpr *>(E));
    case ExprKind::ArrayLiteral:
      return getDerived().visit(static_cast<ArrayLiteralExpr *>(E));
    case ExprKind::Identifier:
      return getDerived().visit(static_cast<IdentifierExpr *>(E));
    case ExprKind::Paren:
      return getDerived().visit(static_cast<ParenExpr *>(E));
    case ExprKind::Assign:
      return getDerived().visit(static_cast<AssignExpr *>(E));
    case ExprKind::Infix:
      return getDerived().visit(static_cast<InfixExpr *>(E));
    case ExprKind::Prefix:
      return getDerived().visit(static_cast<PrefixExpr *>(E));
    case ExprKind::Call:
      return getDerived().visit(static_cast<CallExpr *>(E));
    case ExprKind::Subscript:
      return getDerived().visit(static_cast<SubscriptExpr *>(E));
    }
  }

  /// Visit a concrete statement node.
  bool visit_(Stmt *S) {
    switch (S->getKind()) {
    case StmtKind::Break:
      return getDerived().visit(static_cast<BreakStmt *>(S));
    case StmtKind::Return:
      return getDerived().visit(static_cast<ReturnStmt *>(S));
    case StmtKind::Range:
      return getDerived().visit(static_cast<RangeStmt *>(S));
    case StmtKind::Subscript:
      return getDerived().visit(static_cast<SubscriptStmt *>(S));
    case StmtKind::Block:
      return getDerived().visit(static_cast<BlockStmt *>(S));
    case StmtKind::Extern:
      return getDerived().visit(static_cast<ExternStmt *>(S));
    case StmtKind::For:
      return getDerived().visit(static_cast<ForStmt *>(S));
    case StmtKind::Func:
      return getDerived().visit(static_cast<FuncStmt *>(S));
    case StmtKind::If:
      return getDerived().visit(static_cast<IfStmt *>(S));
    case StmtKind::While:
      return getDerived().visit(static_cast<WhileStmt *>(S));
    }
  }

  /// Visit a concrete pattern node.
  bool visit_(Pattern *P) {
    switch (P->getKind()) {
    case PatternKind::Expr:
      return getDerived().visit(static_cast<ExprPattern *>(P));
    case PatternKind::Variable:
      return getDerived().visit(static_cast<VarPattern *>(P));
    }
  }
  
  /// Visits concrete TypeRepr
  bool visit_(TypeRepr *T) {
    switch (T->getKind()) {
    case TypeReprKind::Ident:
      return getDerived().visit(static_cast<IdentTypeRepr *>(T));
    case TypeReprKind::Array:
      return getDerived().visit(static_cast<ArrayTypeRepr *>(T));
    }
  }
  
  /// Visit a concrete declaration node.
  bool visit(Decl *D) {
    switch (D->getKind()) {
    case DeclKind::Let:
      return getDerived().visitLetDecl(static_cast<LetDecl *>(D));
    case DeclKind::Var:
      return getDerived().visitVarDecl(static_cast<VarDecl *>(D));
    case DeclKind::Func:
      return getDerived().visitFuncDecl(static_cast<FuncDecl *>(D));
    case DeclKind::Module:
      return getDerived().visitModuleDecl(static_cast<ModuleDecl *>(D));
    case DeclKind::Param:
      return getDerived().visitParamDecl(static_cast<ParamDecl *>(D));
    }
  }
  
  /// Visit a concrete expression node.
  Expr *visit(Expr *E) {
    switch (E->getKind()) {
    case ExprKind::NumberLiteral:
      return getDerived().visitNumberLiteralExpr(
          static_cast<NumberLiteralExpr *>(E));
    case ExprKind::ArrayLiteral:
      return getDerived().visitArrayLiteralExpr(
          static_cast<ArrayLiteralExpr *>(E));
    case ExprKind::Identifier:
      return getDerived().visitIdentifierExpr(static_cast<IdentifierExpr *>(E));
    case ExprKind::Paren:
      return getDerived().visitParenExpr(static_cast<ParenExpr *>(E));
    case ExprKind::Assign:
      return getDerived().visitAssignExpr(static_cast<AssignExpr *>(E));
    case ExprKind::Infix:
      return getDerived().visitInfixExpr(static_cast<InfixExpr *>(E));
    case ExprKind::Prefix:
      return getDerived().visitPrefixExpr(static_cast<PrefixExpr *>(E));
    case ExprKind::Call:
      return getDerived().visitCallExpr(static_cast<CallExpr *>(E));
    case ExprKind::Subscript:
      return getDerived().visitSubscriptExpr(static_cast<SubscriptExpr *>(E));
    }
  }
  
  /// Visit a concrete statement node.
  bool visit(Stmt *S) {
    switch (S->getKind()) {
    case StmtKind::Break:
      return getDerived().visitBreakStmt(static_cast<BreakStmt *>(S));
    case StmtKind::Return:
      return getDerived().visitReturnStmt(static_cast<ReturnStmt *>(S));
    case StmtKind::Range:
      return getDerived().visitRangeStmt(static_cast<RangeStmt *>(S));
    case StmtKind::Subscript:
      return getDerived().visitSubscriptStmt(static_cast<SubscriptStmt *>(S));
    case StmtKind::Block:
      return getDerived().visitBlockStmt(static_cast<BlockStmt *>(S));
    case StmtKind::Extern:
      return getDerived().visitExternStmt(static_cast<ExternStmt *>(S));
    case StmtKind::For:
      return getDerived().visitForStmt(static_cast<ForStmt *>(S));
    case StmtKind::Func:
      return getDerived().visitFuncStmt(static_cast<FuncStmt *>(S));
    case StmtKind::If:
      return getDerived().visitIfStmt(static_cast<IfStmt *>(S));
    case StmtKind::While:
      return getDerived().visitWhileStmt(static_cast<WhileStmt *>(S));
    }
  }
  
  /// Visit a concrete pattern node.
  bool visit(Pattern *P) {
    switch (P->getKind()) {
    case PatternKind::Expr:
      return getDerived().visitExprPattern(static_cast<ExprPattern *>(P));
    case PatternKind::Variable:
      return getDerived().visitVarPattern(static_cast<VarPattern *>(P));
    }
  }
  
  /// Visits concrete TypeRepr
  bool visit(TypeRepr *T) {
    switch (T->getKind()) {
    case TypeReprKind::Ident:
      return getDerived().visitIdentTypeRepr(static_cast<IdentTypeRepr *>(T));
    case TypeReprKind::Array:
      return getDerived().visitArrayTypeRepr(static_cast<ArrayTypeRepr *>(T));
    }
  }
};

} // namespace dusk

#endif /* DUSK_AST_VISITOR_H */
