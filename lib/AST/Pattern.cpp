//===--- Stmt.cpp ---------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/Pattern.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"

using namespace dusk;

// MARK: - Pattern

Pattern::Pattern(PatternKind K) : Kind(K), Ty(nullptr) {}

// MARK: - Expression pattern

ExprPattern::ExprPattern(SmallVector<Expr *, 128> &&V, SMLoc L, SMLoc R)
    : Pattern(PatternKind::Expr), Values(V), LPar(L), RPar(R) {}

SMRange ExprPattern::getSourceRange() const { return {LPar, RPar}; }
size_t ExprPattern::count() const { return Values.size(); }

// MARK: - Variable pattern

VarPattern::VarPattern(SmallVector<Decl *, 128> &&V, SMLoc L, SMLoc R)
    : Pattern(PatternKind::Variable), Vars(V), LPar(L), RPar(R) {}

SMRange VarPattern::getSourceRange() const { return {LPar, RPar}; }
size_t VarPattern::count() const { return Vars.size(); }
