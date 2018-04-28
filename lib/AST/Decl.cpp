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

Decl::Decl(DeclKind K, StringRef N, SMLoc NL)
    : Kind(K), Name(N), NameLoc(NL), Ty(nullptr), TyRepr(nullptr) {}

Decl::Decl(DeclKind K, StringRef N, SMLoc NL, TypeRepr *TR) : Decl(K, N, NL) {
  TyRepr = TR;
}

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

ValDecl::ValDecl(DeclKind K, StringRef N, SMLoc NL, Expr *E, TypeRepr *TR)
    : Decl(K, N, NL, TR), Value(E) {}

// MARK: - VarDecl class

VarDecl::VarDecl(StringRef N, SMLoc NL, SMLoc VarL, Expr *V)
    : ValDecl(DeclKind::Var, N, NL, V), VarLoc(VarL) {}

VarDecl::VarDecl(StringRef N, SMLoc NL, SMLoc VarL, Expr *V, TypeRepr *TR)
    : ValDecl(DeclKind::Var, N, NL, V, TR), VarLoc(VarL) {}

SMRange VarDecl::getSourceRange() const {
  if (hasValue())
    return {VarLoc, getValue()->getLocEnd()};
  else if (hasTypeRepr())
    return {VarLoc, getTypeRepr()->getLocEnd()};
  else
    return {VarLoc, Decl::getSourceRange().End};
}

// MARK: - LetDecl class

LetDecl::LetDecl(StringRef N, SMLoc NL, SMLoc ConstL, Expr *V)
    : ValDecl(DeclKind::Let, N, NL, V), LetLoc(ConstL) {}

LetDecl::LetDecl(StringRef N, SMLoc NL, SMLoc ConstL, Expr *V, TypeRepr *TR)
    : ValDecl(DeclKind::Let, N, NL, V, TR), LetLoc(ConstL) {}

SMRange LetDecl::getSourceRange() const {
  if (hasValue())
    return {LetLoc, getValue()->getLocEnd()};
  else
    return Decl::getSourceRange();
}

// MARK: - ParamDecl class

ParamDecl::ParamDecl(StringRef N, SMLoc NL) : Decl(DeclKind::Param, N, NL) {}
ParamDecl::ParamDecl(StringRef N, SMLoc NL, TypeRepr *TR)
    : Decl(DeclKind::Param, N, NL, TR) {}

// MARK: - FuncDecl class

FuncDecl::FuncDecl(StringRef N, SMLoc NL, SMLoc FuncL, VarPattern *A,
                   TypeRepr *TR)
    : Decl(DeclKind::Func, N, NL, TR), FuncLoc(FuncL), Params(A) {}

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
