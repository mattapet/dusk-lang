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

BreakStmt::BreakStmt(llvm::SMRange BL) : Stmt(StmtKind::Break), BreakLoc(BL) {}

llvm::SMRange BreakStmt::getSourceRange() const { return BreakLoc; }

// MARK: - Return statement

ReturnStmt::ReturnStmt(llvm::SMLoc RL, Expr *V)
    : Stmt(StmtKind::Return), RetLoc(RL), Value(V) {}

llvm::SMRange ReturnStmt::getSourceRange() const {
  return {RetLoc, Value->getLocEnd()};
}

// MARK: - Range statement

RangeStmt::RangeStmt(Expr *S, Expr *E, Token O)
    : Stmt(StmtKind::Range), Start(S), End(E), Op(O) {}

bool RangeStmt::isInclusive() const { return Op.is(tok::elipsis_incl); }

llvm::SMRange RangeStmt::getSourceRange() const {
  return {Start->getLocStart(), End->getLocEnd()};
}

// MARK: - Block statement

BlockStmt::BlockStmt(llvm::SMLoc S, llvm::SMLoc E, std::vector<ASTNode *> &&N)
    : Stmt(StmtKind::Block), BlockStart(S), BlockEnd(E), Nodes(N) {}

llvm::SMRange BlockStmt::getSourceRange() const {
  return {BlockStart, BlockEnd};
}

// MARK: - Funcion statement

FuncStmt::FuncStmt(Decl *FP, Stmt *B)
    : Stmt(StmtKind::Func), Prototype(FP), Body(B) {}

llvm::SMRange FuncStmt::getSourceRange() const {
  return {Prototype->getLocStart(), Body->getLocEnd()};
}

// MARK: Fot-in statement

ForStmt::ForStmt(llvm::SMLoc FL, Expr *V, Stmt *R, Stmt *B)
    : Stmt(StmtKind::For), ForLoc(FL), Var(V), Range(R), Body(B) {}

llvm::SMRange ForStmt::getSourceRange() const {
  return {ForLoc, Body->getLocEnd()};
}

// MARK: - While statement

WhileStmt::WhileStmt(llvm::SMLoc WL, Expr *C, Stmt *B)
    : Stmt(StmtKind::While), WhileLoc(WL), Cond(C), Body(B) {}

llvm::SMRange WhileStmt::getSourceRange() const {
  return {WhileLoc, Body->getLocEnd()};
}

// MARK: - If statement

IfStmt::IfStmt(llvm::SMLoc IL, Expr *C, Stmt *T, Stmt *E)
    : Stmt(StmtKind::If), IfLoc(IL), Cond(C), Then(T), Else(E) {}

llvm::SMRange IfStmt::getSourceRange() const {
  if (Else != nullptr)
    return {IfLoc, Else->getLocEnd()};
  else
    return {IfLoc, Then->getLocEnd()};
}
