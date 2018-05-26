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
    : S(S), Lookup(C), Ctx(Ctx), Diag(D) {}

void TypeChecker::diagnose(SMLoc Loc, diag::DiagID ID) {
  Diag.diagnose(Loc, ID);
  Ctx.setError();
}

namespace  {
  
/// Walks expression tree while resolving potentially invalid use of immutable
/// variables.
class MutationResolver : public ASTWalker {
  TypeChecker &TC;
  
public:
  MutationResolver(TypeChecker &TC) : TC(TC) {}
  
  Expr *postWalkExpr(Expr *E) {
    if (auto Ident = dynamic_cast<IdentifierExpr *>(E))
      if (auto D = static_cast<ValDecl *>(TC.Lookup.getVal(Ident->getName())))
        if (D->isLet())
          TC.diagnose(E->getLocStart(), diag::cannot_reassign_let_value);
    return E;
  }
  
  // Skip all statements
  bool preWalkStmt(Stmt *S) { return false; }
};
  
} // anonymous namespace

void TypeChecker::ensureMutable(Expr *E) {
  MutationResolver MR(*this);
  E->walk(MR);
}
