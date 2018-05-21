//===--- TypeResolver.h - Type resolution -----------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_SEMA_TYPE_RESOLVER_H
#define DUSK_SEMA_TYPE_RESOLVER_H

#include "dusk/AST/ASTWalker.h"

namespace dusk {
class NumberLiteralExpr;
class ArrayLiteralExpr;
class IdentifierExpr;
class ParenExpr;
class AssignExpr;
class InfixExpr;
class PrefixExpr;
class CallExpr;
class SubscriptExpr;
class ASTcontext;
class NameLookup;
class DiagnosticEngine;

namespace sema {

class TypeResolver : public ASTWalker {
  ASTContext &Context;
  NameLookup &NL;
  DiagnosticEngine &Diag;

public:
  TypeResolver(ASTContext &Ctx, NameLookup &NL, DiagnosticEngine &D);

  bool preWalkDecl(Decl *D) override;
  bool postWalkDecl(Decl *D) override;

  std::pair<bool, Expr *> preWalkExpr(Expr *E) override;
  Expr *postWalkExpr(Expr *E) override;

  bool preWalkTypeRepr(TypeRepr *TR) override;
  bool postWalkTypeRepr(TypeRepr *TR) override;

private:
  bool resolveLetDecl(LetDecl *D);
  bool resolveFuncDecl(FuncDecl *D);
  bool resolveModuleDecl(ModuleDecl *D);
  bool resolveParamDecl(ParamDecl *D);
  bool resolveVarDecl(VarDecl *D);

  Expr *resolveNumberLiteralExpr(NumberLiteralExpr *E);
  Expr *resolveArrayLiteralExpr(ArrayLiteralExpr *E);
  Expr *resolveIdentifierExpr(IdentifierExpr *E);
  Expr *resolveParenExpr(ParenExpr *E);
  Expr *resolveAssignExpr(AssignExpr *E);
  Expr *resolveInfixExpr(InfixExpr *E);
  Expr *resolvePrefixExpr(PrefixExpr *E);
  Expr *resolveCallExpr(CallExpr *E);
  Expr *resolveSubscriptExpr(SubscriptExpr *E);

  bool typeResolveIdentTypeRepr(IdentTypeRepr *TR);
  bool typeResolveArrayTypeRepr(ArrayTypeRepr *TR);
};

} // namespace sema
} // namespace dusk

#endif /* DUSK_SEMA_TYPE_RESOLVER_H */
