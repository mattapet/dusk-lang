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
: ListStart(LS), ListEnd(LE), Params(P)
{
    for (auto Param : Params)
        assert(Param && "Invalid `ParamList` statement.");
}

llvm::SMRange ParamList::getSourceRange() const {
    return { ListStart, ListEnd };
}

bool ParamList::walk(ASTWalker &Walker) {
    if (!Walker.preWalk(this))
        return false;
    for (auto Param : Params)
        if (!Param->walk(Walker))
            return false;
    if (!Walker.postWalk(this))
        return false;
    return true;
}

// MARK: - Code block statement

CodeBlock::CodeBlock(llvm::SMLoc S, llvm::SMLoc E, std::vector<ASTNode *> &&N)
: BlockStart(S), BlockEnd(E), Nodes(N)
{
    for (const auto Node : Nodes)
        assert(Node && "Invalid `CodeBlock` statement.");
}

llvm::SMRange CodeBlock::getSourceRange() const {
    return { BlockStart, BlockEnd };
}

bool CodeBlock::walk(ASTWalker &Walker) {
    if (!Walker.preWalk(this))
        return false;
    for (auto Node : Nodes)
        if (!Node->walk(Walker))
            return false;
    if (!Walker.postWalk(this))
        return false;
    return true;
}

// MARK: - FuncStmt

FuncStmt::FuncStmt(FuncDecl *FP, CodeBlock *B)
: Prototype(FP), Body(B)
{
    assert(Prototype && Body && "Invalid `func` statement");
}

llvm::SMRange FuncStmt::getSourceRange() const {
    return { Prototype->getLocStart(), Body->getLocEnd() };
}

bool FuncStmt::walk(ASTWalker &Walker) {
    if (!Walker.preWalk(this))
        return false;
    if (!Prototype->walk(Walker))
        return false;
    if (!Body->walk(Walker))
        return false;
    if (!Walker.postWalk(this))
        return false;
    return true;
}

// MARK: - While statement

WhileStmt::WhileStmt(llvm::SMLoc WL, Expr *C, CodeBlock *B)
: WhileLoc(WL), Cond(C), Body(B)
{
    assert(C && B && "Invalid `while` statement");
}

llvm::SMRange WhileStmt::getSourceRange() const {
    return { WhileLoc, Body->getLocEnd() };
}

bool WhileStmt::walk(ASTWalker &Walker) {
    if (!Walker.preWalk(this))
        return false;
    if (!Cond->walk(Walker))
        return false;
    if (!Body->walk(Walker))
        return false;
    if (!Walker.postWalk(this))
        return false;
    return true;
}


// MARK: - If statement

IfStmt::IfStmt(llvm::SMLoc IL, Expr *C, CodeBlock *T, CodeBlock *E)
: IfLoc(IL), Cond(C), Then(T), Else(E)
{
    assert(C && T && "Invalid `if` statement");
}

llvm::SMRange IfStmt::getSourceRange() const {
    if (Else != nullptr)
        return { IfLoc, Else->getLocEnd() };
    else
        return { IfLoc, Then->getLocEnd() };
}

bool IfStmt::walk(ASTWalker &Walker) {
    if (!Walker.preWalk(this))
        return false;
    if (!Cond->walk(Walker))
        return false;
    if (!Cond->walk(Walker))
        return false;
    if (!Then->walk(Walker))
        return false;
    if (Else && !Else->walk(Walker))
        return false;
    if (!Walker.postWalk(this))
        return false;
    return true;
}
