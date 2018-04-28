//===--- Stmt.cpp ---------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/Stmt.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/ASTWalker.h"

using namespace dusk;

// MARK: - Break statement

BreakStmt::BreakStmt(SMRange BL) : Stmt(StmtKind::Break), BreakLoc(BL) {}

SMRange BreakStmt::getSourceRange() const { return BreakLoc; }

// MARK: - Return statement

ReturnStmt::ReturnStmt(SMLoc RL, Expr *V)
    : Stmt(StmtKind::Return), RetLoc(RL), Value(V) {}

SMRange ReturnStmt::getSourceRange() const {
  if (hasValue())
    return {RetLoc, Value->getLocEnd()};
  else
    return {RetLoc, RetLoc};
}

// MARK: - Range statement

RangeStmt::RangeStmt(Expr *S, Expr *E, Token O)
    : Stmt(StmtKind::Range), Start(S), End(E), Op(O) {}

bool RangeStmt::isInclusive() const { return Op.is(tok::elipsis_incl); }

SMRange RangeStmt::getSourceRange() const {
  return {Start->getLocStart(), End->getLocEnd()};
}

// MARK: - Subcsript statement
SubscriptStmt::SubscriptStmt(Expr *V, SMLoc L, SMLoc R)
    : Stmt(StmtKind::Subscript), Value(V), LBracket(L), RBracket(R) {}

SMRange SubscriptStmt::getSourceRange() const { return {LBracket, RBracket}; }

// MARK: - Block statement

BlockStmt::BlockStmt(SMLoc S, SMLoc E, std::vector<ASTNode *> &&N)
    : Stmt(StmtKind::Block), BlockStart(S), BlockEnd(E), Nodes(N) {}

SMRange BlockStmt::getSourceRange() const { return {BlockStart, BlockEnd}; }

// MARK: - Extern statement

ExternStmt::ExternStmt(SMLoc EL, Decl *P)
    : Stmt(StmtKind::Extern), ExternLoc(EL), Prototype(P) {}

SMRange ExternStmt::getSourceRange() const {
  return { ExternLoc, Prototype->getSourceRange().End };
}

// MARK: - Funcion statement

FuncStmt::FuncStmt(Decl *FP, Stmt *B)
    : Stmt(StmtKind::Func), Prototype(FP), Body(B) {}

SMRange FuncStmt::getSourceRange() const {
  return {Prototype->getLocStart(), Body->getLocEnd()};
}

// MARK: Fot-in statement

ForStmt::ForStmt(SMLoc FL, Expr *V, Stmt *R, Stmt *B)
    : Stmt(StmtKind::For), ForLoc(FL), Var(V), Range(R), Body(B) {}

SMRange ForStmt::getSourceRange() const { return {ForLoc, Body->getLocEnd()}; }

// MARK: - While statement

WhileStmt::WhileStmt(SMLoc WL, Expr *C, Stmt *B)
    : Stmt(StmtKind::While), WhileLoc(WL), Cond(C), Body(B) {}

SMRange WhileStmt::getSourceRange() const {
  return {WhileLoc, Body->getLocEnd()};
}

// MARK: - If statement

IfStmt::IfStmt(SMLoc IL, Expr *C, Stmt *T, Stmt *E)
    : Stmt(StmtKind::If), IfLoc(IL), Cond(C), Then(T), Else(E) {}

SMRange IfStmt::getSourceRange() const {
  if (Else != nullptr)
    return {IfLoc, Else->getLocEnd()};
  else
    return {IfLoc, Then->getLocEnd()};
}
