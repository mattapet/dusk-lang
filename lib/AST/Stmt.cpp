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

// MARK: - Parameter list statement

ParamList::ParamList(llvm::SMLoc LS, llvm::SMLoc LE,
                     llvm::SmallVector<ParamDecl *, 128> &&P)
: Stmt(StmtKind::ParamList), ListStart(LS), ListEnd(LE), Params(P)
{
    for (auto Param : Params)
        assert(Param && "Invalid `ParamList` statement.");
}

llvm::SMRange ParamList::getSourceRange() const {
    return { ListStart, ListEnd };
}

// MARK: - Code block statement

CodeBlock::CodeBlock(llvm::SMLoc S, llvm::SMLoc E, std::vector<ASTNode *> &&N)
: Stmt(StmtKind::CodeBlock), BlockStart(S), BlockEnd(E), Nodes(N)
{
    for (const auto Node : Nodes)
        assert(Node && "Invalid `CodeBlock` statement.");
}

llvm::SMRange CodeBlock::getSourceRange() const {
    return { BlockStart, BlockEnd };
}

// MARK: - FuncStmt

FuncStmt::FuncStmt(FuncDecl *FP, CodeBlock *B)
: Stmt(StmtKind::Func), Prototype(FP), Body(B)
{
    assert(Prototype && Body && "Invalid `func` statement");
}

llvm::SMRange FuncStmt::getSourceRange() const {
    return { Prototype->getLocStart(), Body->getLocEnd() };
}

// MARK: Fot-in statement

ForStmt::ForStmt(llvm::SMLoc FL, llvm::SMLoc IL,
                 llvm::StringRef IN, InfixExpr *R, CodeBlock *B)
: Stmt(StmtKind::For), ForLoc(FL), ItLoc(IL), ItName(IN), Range(R), Body(B)
{
    assert(Range && Body && "Invalid `for-in` statement");
}

llvm::SMRange ForStmt::getSourceRange() const {
    return { ForLoc, Body->getLocEnd() };
}

// MARK: - While statement

WhileStmt::WhileStmt(llvm::SMLoc WL, Expr *C, CodeBlock *B)
: Stmt(StmtKind::While), WhileLoc(WL), Cond(C), Body(B)
{
    assert(C && B && "Invalid `while` statement");
}

llvm::SMRange WhileStmt::getSourceRange() const {
    return { WhileLoc, Body->getLocEnd() };
}


// MARK: - If statement

IfStmt::IfStmt(llvm::SMLoc IL, Expr *C, CodeBlock *T, CodeBlock *E)
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
