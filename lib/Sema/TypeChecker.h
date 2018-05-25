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
class TypeChecker {
  Sema &S;

public:
  NameLookup &Lookup;
  ASTContext &Ctx;
  Scope ASTScope;
  DiagnosticEngine &Diag;

  TypeChecker(Sema &S, NameLookup &DC, ASTContext &Ctx, DiagnosticEngine &Diag);

  void diagnose(SMLoc Loc, diag::DiagID ID);

  bool typeCheckEquals(Type *LHS, Type *RHS) { return LHS->isClassOf(RHS); }

  void ensureMutable(Expr *E);

  void typeCheckDecl(Decl *D);
  Expr *typeCheckExpr(Expr *E);
  void typeCheckStmt(Stmt *S);
  void typeCheckPattern(Pattern *P);
  void typeCheckType(TypeRepr *TR);
};

} // namespace sema

} // namespace dusk

#endif /* DUSK_SEMA_TYPE_CHECKER_H */
