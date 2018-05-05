//===--- TypeChecker.cpp --------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "TypeChecker.h"

#include "dusk/AST/Diagnostics.h"
#include "dusk/AST/Scope.h"
#include "dusk/AST/NameLookup.h"
#include "dusk/Sema/Sema.h"

using namespace dusk;
using namespace sema;

TypeChecker::TypeChecker(Sema &S, NameLookup &C, ASTContext &Ctx,
                         DiagnosticEngine &D)
    : S(S), DeclCtx(C), Ctx(Ctx), Diag(D) {
  Scp.push(Scope());
}

void TypeChecker::diagnose(SMLoc Loc, diag::DiagID ID) {
  Diag.diagnose(Loc, ID);
  Ctx.setError();
}

bool TypeChecker::preWalk(Decl *D) {
  switch (D->getKind()) {
  case DeclKind::Let:
    return preWalkLetDecl(static_cast<LetDecl *>(D));
  case DeclKind::Func:
    return preWalkFuncDecl(static_cast<FuncDecl *>(D));
  case DeclKind::Module:
    return preWalkModuleDecl(static_cast<ModuleDecl *>(D));
  case DeclKind::Param:
    return preWalkParamDecl(static_cast<ParamDecl *>(D));
  case DeclKind::Var:
    return preWalkVarDecl(static_cast<VarDecl *>(D));
  }
}

bool TypeChecker::postWalk(Decl *D) {
  switch (D->getKind()) {
  case DeclKind::Let:
    return postWalkLetDecl(static_cast<LetDecl *>(D));
  case DeclKind::Func:
    return postWalkFuncDecl(static_cast<FuncDecl *>(D));
  case DeclKind::Module:
    return postWalkModuleDecl(static_cast<ModuleDecl *>(D));
  case DeclKind::Param:
    return postWalkParamDecl(static_cast<ParamDecl *>(D));
  case DeclKind::Var:
    return postWalkVarDecl(static_cast<VarDecl *>(D));
  }
}

bool TypeChecker::preWalk(Expr *E) {
  // Skip expression type validation tree if the root expression has a type.
  return E->getType() == nullptr;
}

bool TypeChecker::postWalk(Expr *E) {
  switch (E->getKind()) {
  case ExprKind::NumberLiteral:
    return postWalkNumberLiteralExpr(static_cast<NumberLiteralExpr *>(E));
  case ExprKind::Identifier:
    return postWalkIdentifierExpr(static_cast<IdentifierExpr *>(E));
  case ExprKind::Paren:
    return postWalkParenExpr(static_cast<ParenExpr *>(E));
  case ExprKind::Assign:
    return postWalkAssignExpr(static_cast<AssignExpr *>(E));
  case ExprKind::Infix:
    return postWalkInfixExpr(static_cast<InfixExpr *>(E));
  case ExprKind::Prefix:
    return postWalkPrefixExpr(static_cast<PrefixExpr *>(E));
  case ExprKind::Call:
    return postWalkCallExpr(static_cast<CallExpr *>(E));
  case ExprKind::Subscript:
    return postWalkSubscriptExpr(static_cast<SubscriptExpr *>(E));
  }
}

bool TypeChecker::preWalk(Stmt *S) {
  switch (S->getKind()) {
  case StmtKind::Block:
    return preWalkBlockStmt(static_cast<BlockStmt *>(S));
  case StmtKind::Extern:
    return preWalkExternStmt(static_cast<ExternStmt *>(S));
  case StmtKind::For:
    return preWalkForStmt(static_cast<ForStmt *>(S));
  case StmtKind::Func:
    return preWalkFuncStmt(static_cast<FuncStmt *>(S));
  case StmtKind::If:
    return preWalkIfStmt(static_cast<IfStmt *>(S));
  case StmtKind::While:
    return preWalkWhileStmt(static_cast<WhileStmt *>(S));
  default: return true;
  }
}

bool TypeChecker::postWalk(Stmt *S) {
  switch (S->getKind()) {
  case StmtKind::Break:
    return postWalkBreakStmt(static_cast<BreakStmt *>(S));
  case StmtKind::Return:
    return postWalkReturnStmt(static_cast<ReturnStmt *>(S));
  case StmtKind::Range:
    return postWalkRangeStmt(static_cast<RangeStmt *>(S));
  case StmtKind::Subscript:
    return postWalkSubscriptStmt(static_cast<SubscriptStmt *>(S));
  case StmtKind::Block:
    return postWalkBlockStmt(static_cast<BlockStmt *>(S));
  case StmtKind::Extern:
    return postWalkExternStmt(static_cast<ExternStmt *>(S));
  case StmtKind::For:
    return postWalkForStmt(static_cast<ForStmt *>(S));
  case StmtKind::Func:
    return postWalkFuncStmt(static_cast<FuncStmt *>(S));
  case StmtKind::If:
    return postWalkIfStmt(static_cast<IfStmt *>(S));
  case StmtKind::While:
    return postWalkWhileStmt(static_cast<WhileStmt *>(S));
  }
}

bool TypeChecker::preWalk(Pattern *P) {
  switch (P->getKind()) {
  case PatternKind::Variable:
    return preWalkVarPattern(static_cast<VarPattern *>(P));
  case PatternKind::Expr:
    return preWalkExprPattern(static_cast<ExprPattern *>(P));
  }
}

bool TypeChecker::postWalk(Pattern *P) {
  switch (P->getKind()) {
    case PatternKind::Variable:
      return postWalkVarPattern(static_cast<VarPattern *>(P));
    case PatternKind::Expr:
      return postWalkExprPattern(static_cast<ExprPattern *>(P));
  }
}


