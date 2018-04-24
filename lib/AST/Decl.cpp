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
#include "dusk/AST/Pattern.h"
#include "dusk/AST/Type.h"

using namespace dusk;

// MARK: - Decl class

Decl::Decl(DeclKind K, StringRef N, SMLoc NL) : Kind(K), Name(N), NameLoc(NL) {}

bool Decl::isValDecl() const {
  switch (Kind) {
  case DeclKind::Var:
  case DeclKind::Let:
  case DeclKind::Param:
    return true;
  default:
    return false;
  }
}

SMRange Decl::getSourceRange() const {
  auto EndLoc = Name.data() + Name.size();
  return {NameLoc, SMLoc::getFromPointer(EndLoc)};
}

// MARK: - ValDecl class

ValDecl::ValDecl(DeclKind K, StringRef N, SMLoc NL, Expr *E)
    : Decl(K, N, NL), Value(E) {}

// MARK: - VarDecl class

VarDecl::VarDecl(StringRef N, SMLoc NL, SMLoc VarL, Expr *V)
    : ValDecl(DeclKind::Var, N, NL, V), VarLoc(VarL) {}

SMRange VarDecl::getSourceRange() const {
  return {VarLoc, getValue()->getLocEnd()};
}

// MARK: - LetDecl class

LetDecl::LetDecl(StringRef N, SMLoc NL, SMLoc ConstL, Expr *V)
    : ValDecl(DeclKind::Let, N, NL, V), LetLoc(ConstL) {}

SMRange LetDecl::getSourceRange() const {
  return {LetLoc, getValue()->getLocEnd()};
}

// MARK: - ParamDecl class

ParamDecl::ParamDecl(StringRef N, SMLoc NL) : Decl(DeclKind::Param, N, NL) {}

// MARK: - FuncDecl class

FuncDecl::FuncDecl(StringRef N, SMLoc NL, SMLoc FuncL, VarPattern *A,
                   FuncRetType *R)
    : Decl(DeclKind::Func, N, NL), FuncLoc(FuncL), Params(A), RetTy(R) {}

SMRange FuncDecl::getSourceRange() const {
  return {FuncLoc, Params->getLocEnd()};
}

// MARK: - Module declaration

ModuleDecl::ModuleDecl(StringRef N, std::vector<ASTNode *> &&C)
    : Decl(DeclKind::Module, N, SMLoc()), Contents(C) {}

SMRange ModuleDecl::getSourceRange() const {
  if (Contents.size() > 0) {
    return {Contents.front()->getLocStart(), Contents.back()->getLocEnd()};
  }
  return SMRange();
}
