//===--- TypeChecker.h - Dusk type analyzer ---------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_SEMA_TYPE_CHECKER_H
#define DUSK_SEMA_TYPE_CHECKER_H

#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/ASTContext.h"
#include "dusk/AST/ASTWalker.h"
#include "dusk/AST/Scope.h"
#include <stack>

namespace dusk {
class Type;
class TypeRepr;
class DiagnosticEngine;
class Scope;
class NameLookup;

namespace diag {
  enum DiagID : unsigned;
}
  
namespace sema {
class Sema;

/// Dusk language type checker.
///
/// This class takes an AST as an input and resolves types of all it's nodes,
/// while validating them.
class TypeChecker /*: public ASTWalker*/ {
  Sema &S;
  
public:
  NameLookup &Lookup;
  ASTContext &Ctx;
  std::stack<Scope> Scp;
  Scope ASTScope;
  DiagnosticEngine &Diag;
  
  TypeChecker(Sema &S, NameLookup &DC, ASTContext &Ctx, DiagnosticEngine &Diag);

//  virtual bool preWalk(Decl *D) override;
//  virtual bool postWalk(Decl *D) override;
//
//  virtual bool preWalk(Expr *E) override;
//  virtual bool postWalk(Expr *E) override;
//
//  virtual bool preWalk(Stmt *S) override;
//  virtual bool postWalk(Stmt *S) override;
//
//  virtual bool preWalk(Pattern *P) override;
//  virtual bool postWalk(Pattern *P) override;

  void diagnose(SMLoc Loc, diag::DiagID ID);
  
  bool typeCheckEquals(Type *LHS, Type *RHS) {
    return LHS->isClassOf(RHS);
  }
  
  void typeCheckDecl(Decl *D);
  Expr *typeCheckExpr(Expr *E);
  void typeCheckStmt(Stmt *S);
  void typeCheckPattern(Pattern *P);
  void typeCheckType(TypeRepr *TR);
  
//private:
//  // MARK: - Declarations
//  bool preWalkLetDecl(LetDecl *D);
//  bool preWalkFuncDecl(FuncDecl *D);
//  bool preWalkModuleDecl(ModuleDecl *D);
//  bool preWalkParamDecl(ParamDecl *D);
//  bool preWalkVarDecl(VarDecl *D);
//
//  bool postWalkLetDecl(LetDecl *D);
//  bool postWalkFuncDecl(FuncDecl *D);
//  bool postWalkModuleDecl(ModuleDecl *D);
//  bool postWalkParamDecl(ParamDecl *D);
//  bool postWalkVarDecl(VarDecl *D);
//
//  // MARK: - Expressions
//  bool postWalkNumberLiteralExpr(NumberLiteralExpr *E);
//  bool postWalkArrayLiteralExpr(ArrayLiteralExpr *E);
//  bool postWalkIdentifierExpr(IdentifierExpr *E);
//  bool postWalkParenExpr(ParenExpr *E);
//  bool postWalkAssignExpr(AssignExpr *E);
//  bool postWalkInfixExpr(InfixExpr *E);
//  bool postWalkPrefixExpr(PrefixExpr *E);
//  bool postWalkCallExpr(CallExpr *E);
//  bool postWalkSubscriptExpr(SubscriptExpr *E);
//
//  // MARK: - Statements
//  bool preWalkBlockStmt(BlockStmt *S);
//  bool preWalkExternStmt(ExternStmt *S);
//  bool preWalkForStmt(ForStmt *S);
//  bool preWalkFuncStmt(FuncStmt *S);
//  bool preWalkIfStmt(IfStmt *S);
//  bool preWalkWhileStmt(WhileStmt *S);
//
//  bool postWalkBreakStmt(BreakStmt *S);
//  bool postWalkReturnStmt(ReturnStmt *S);
//  bool postWalkRangeStmt(RangeStmt *S);
//  bool postWalkSubscriptStmt(SubscriptStmt *S);
//  bool postWalkBlockStmt(BlockStmt *S);
//  bool postWalkExternStmt(ExternStmt *S);
//  bool postWalkForStmt(ForStmt *S);
//  bool postWalkFuncStmt(FuncStmt *S);
//  bool postWalkIfStmt(IfStmt *S);
//  bool postWalkWhileStmt(WhileStmt *S);
//
//  // MARK: - Patterns
//  bool preWalkVarPattern(VarPattern *P);
//  bool preWalkExprPattern(ExprPattern *P);
//
//  bool postWalkVarPattern(VarPattern *P);
//  bool postWalkExprPattern(ExprPattern *P);
};

} // namespace sema

} // namespace dusk

#endif /* DUSK_SEMA_TYPE_CHECKER_H */
