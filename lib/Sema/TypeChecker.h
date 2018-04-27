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
#include "dusk/AST/ASTWalker.h"

namespace dusk {
class DiagnosticEngine;
  
namespace sema {
class Context;
class Scope;

class TypeChecker : public ASTWalker {
  Context &Ctx;
  Scope &Scp;
  DiagnosticEngine &Diag;

public:
  TypeChecker(Context &Ctx, Scope &S, DiagnosticEngine &Diag);

  virtual bool preWalk(Decl *D) override;
  virtual bool postWalk(Decl *D) override;

  virtual bool preWalk(Expr *E) override;
  virtual bool postWalk(Expr *E) override;

  virtual bool preWalk(Stmt *S) override;
  virtual bool postWalk(Stmt *S) override;

private:
  // MARK: - Declarations
  bool preWalkLetDecl(LetDecl *D);
  bool preWalkFuncDecl(FuncDecl *D);
  bool preWalkModuleDecl(ModuleDecl *D);
  bool preWalkParamDecl(ParamDecl *D);
  bool preWalkVarDecl(VarDecl *D);

  bool postWalkLetDecl(LetDecl *D);
  bool postWalkFuncDecl(FuncDecl *D);
  bool postWalkModuleDecl(ModuleDecl *D);
  bool postWalkParamDecl(ParamDecl *D);
  bool postWalkVarDecl(VarDecl *D);

  // MARK: - Expressions
  bool preWalkNumberLiteralExpr(NumberLiteralExpr *E);
  bool preWalkIdentifierExpr(IdentifierExpr *E);
  bool preWalkParenExpr(ParenExpr *E);
  bool preWalkAssignExpr(AssignExpr *E);
  bool preWalkInfixExpr(InfixExpr *E);
  bool preWalkPrefixExpr(PrefixExpr *E);
  bool preWalkCallExpr(CallExpr *E);
  bool preWalkSubscriptExpr(SubscriptExpr *E);

  bool postWalkNumberLiteralExpr(NumberLiteralExpr *E);
  bool postWalkIdentifierExpr(IdentifierExpr *E);
  bool postWalkParenExpr(ParenExpr *E);
  bool postWalkAssignExpr(AssignExpr *E);
  bool postWalkInfixExpr(InfixExpr *E);
  bool postWalkPrefixExpr(PrefixExpr *E);
  bool postWalkCallExpr(CallExpr *E);
  bool postWalkSubscriptExpr(SubscriptExpr *E);

  // MARK: - Statements
  bool preWalkBreakStmt(BreakStmt *S);
  bool preWalkReturnStmt(ReturnStmt *S);
  bool preWalkRangeStmt(RangeStmt *S);
  bool preWalkSubscriptStmt(SubscriptStmt *S);
  bool preWalkBlockStmt(BlockStmt *S);
  bool preWalkExternStmt(ExternStmt *S);
  bool preWalkForStmt(ForStmt *S);
  bool preWalkFuncStmt(FuncStmt *S);
  bool preWalkIfStmt(IfStmt *S);
  bool preWalkWhileStmt(WhileStmt *S);

  bool postWalkBreakStmt(BreakStmt *S);
  bool postWalkReturnStmt(ReturnStmt *S);
  bool postWalkRangeStmt(RangeStmt *S);
  bool postWalkSubscriptStmt(SubscriptStmt *S);
  bool postWalkBlockStmt(BlockStmt *S);
  bool postWalkExternStmt(ExternStmt *S);
  bool postWalkForStmt(ForStmt *S);
  bool postWalkFuncStmt(FuncStmt *S);
  bool postWalkIfStmt(IfStmt *S);
  bool postWalkWhileStmt(WhileStmt *S);
};

} // namespace sema

} // namespace dusk

#endif /* DUSK_SEMA_TYPE_CHECKER_H */
