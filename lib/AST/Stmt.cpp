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

// MARK: - Range statement

RangeStmt::RangeStmt(Expr *S, Expr *E, Token O)
: Stmt(StmtKind::Range), Start(S), End(E), Op(O)
{
    assert(Start && End && "Invalid `range` expression.");
}

bool RangeStmt::isInclusive() const {
    return Op.is(tok::elipsis_incl);
}

llvm::SMRange RangeStmt::getSourceRange() const {
    return { Start->getLocStart(), End->getLocEnd() };
}

// MARK: - Block statement

BlockStmt::BlockStmt(llvm::SMLoc S, llvm::SMLoc E, std::vector<ASTNode *> &&N)
: Stmt(StmtKind::Block), BlockStart(S), BlockEnd(E), Nodes(N)
{
    for (const auto Node : Nodes)
        assert(Node && "Invalid `CodeBlock` statement.");
}

llvm::SMRange BlockStmt::getSourceRange() const {
    return { BlockStart, BlockEnd };
}

// MARK: - Funcion statement

FuncStmt::FuncStmt(FuncDecl *FP, BlockStmt *B)
: Stmt(StmtKind::Func), Prototype(FP), Body(B)
{
    assert(Prototype && Body && "Invalid `func` statement");
}

llvm::SMRange FuncStmt::getSourceRange() const {
    return { Prototype->getLocStart(), Body->getLocEnd() };
}

// MARK: Fot-in statement

ForStmt::ForStmt(llvm::SMLoc FL, IdentifierExpr *V, RangeStmt *R, BlockStmt *B)
: Stmt(StmtKind::For), ForLoc(FL), Var(V), Range(R), Body(B)
{
    assert(Var && Range && Body && "Invalid `for-in` statement");
}

llvm::SMRange ForStmt::getSourceRange() const {
    return { ForLoc, Body->getLocEnd() };
}

// MARK: - While statement

WhileStmt::WhileStmt(llvm::SMLoc WL, Expr *C, BlockStmt *B)
: Stmt(StmtKind::While), WhileLoc(WL), Cond(C), Body(B)
{
    assert(C && B && "Invalid `while` statement");
}

llvm::SMRange WhileStmt::getSourceRange() const {
    return { WhileLoc, Body->getLocEnd() };
}


// MARK: - If statement

IfStmt::IfStmt(llvm::SMLoc IL, Expr *C, BlockStmt *T, BlockStmt *E)
: Stmt(StmtKind::If), IfLoc(IL), Cond(C), Then(T), Else(E)
{
    assert(C && T && "Invalid `if` statement");
}

llvm::SMRange IfStmt::getSourceRange() const {
    if (Else != nullptr)
        return { IfLoc, Else->getLocEnd() };
    else
        return { IfLoc, Then->getLocEnd() };
}
