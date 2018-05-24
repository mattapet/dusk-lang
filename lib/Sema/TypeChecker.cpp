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
