//===--- Decl.cpp ---------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"

using namespace dusk;


// MARK: - Decl class

Decl::Decl(DeclKind K, llvm::StringRef N, llvm::SMLoc NL)
: Kind(K), Name(N), NameLoc(NL)
{}

bool Decl::isValDecl() const {
    switch (Kind) {
    case DeclKind::Var:
    case DeclKind::Const:
    case DeclKind::Param:
        return true;
    default:
        return false;
    }
}

llvm::SMRange Decl::getSourceRange() const {
    auto EndLoc = Name.data() + Name.size();
    return { NameLoc, llvm::SMLoc::getFromPointer(EndLoc) };
}

// MARK: - ValDecl class

ValDecl::ValDecl(DeclKind K, llvm::StringRef N, llvm::SMLoc NL, Expr *E)
: Decl(K, N, NL), Value(E)
{
    assert(Value && "Invalid `ValDecl` declaraion.");
}

// MARK: - VarDecl class

VarDecl::VarDecl(llvm::StringRef N, llvm::SMLoc NL,
                 llvm::SMLoc VarL, Expr *V)
: ValDecl(DeclKind::Var, N, NL, V), VarLoc(VarL)
{}

llvm::SMRange VarDecl::getSourceRange() const {
    return { VarLoc, getValue()->getLocEnd() };
}


// MARK: - ConstDecl class

ConstDecl::ConstDecl(llvm::StringRef N, llvm::SMLoc NL,
                     llvm::SMLoc ConstL, Expr *V)
: ValDecl(DeclKind::Const, N, NL, V), ConstLoc(ConstL)
{}

llvm::SMRange ConstDecl::getSourceRange() const {
    return { ConstLoc, getValue()->getLocEnd() };
}

// MARK: - ParamDecl class

ParamDecl::ParamDecl(llvm::StringRef N, llvm::SMLoc NL)
: Decl(DeclKind::Param, N, NL)
{}

// MARK: - FuncDecl class

FuncDecl::FuncDecl(llvm::StringRef N, llvm::SMLoc NR,
                   llvm::SMLoc FuncL, ParamList *A)
: Decl(DeclKind::Func, N, NR), FuncLoc(FuncL), Args(A)
{
    assert(Args && "Invalid `FuncDecl` declaration.");
}

llvm::SMRange FuncDecl::getSourceRange() const {
    return { FuncLoc, Args->getLocEnd() };
}
