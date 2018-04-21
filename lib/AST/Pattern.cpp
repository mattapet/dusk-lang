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

Pattern::Pattern(PatternKind K) : Kind(K) {}

// MARK: - Expression pattern

ExprPattern::ExprPattern(llvm::SmallVector<Expr *, 128> &&V, llvm::SMLoc L,
                         llvm::SMLoc R)
    : Pattern(PatternKind::Expr), Values(V), LPar(L), RPar(R) {}

llvm::SMRange ExprPattern::getSourceRange() const { return {LPar, RPar}; }

// MARK: - Variable pattern

VarPattern::VarPattern(llvm::SmallVector<Decl *, 128> &&V, llvm::SMLoc L,
                       llvm::SMLoc R)
    : Pattern(PatternKind::Variable), Vars(V), LPar(L), RPar(R) {}

llvm::SMRange VarPattern::getSourceRange() const { return {LPar, RPar}; }

// MARK: - Subscript pattern

SubscriptPattern::SubscriptPattern(Expr *V, llvm::SMLoc L, llvm::SMLoc R)
    : Pattern(PatternKind::Subscript), Value(V), LBracet(L), RBracet(R) {}

llvm::SMRange SubscriptPattern::getSourceRange() const {
  return {LBracet, RBracet};
}
