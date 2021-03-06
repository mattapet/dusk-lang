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

#define DECL(CLASS, PARENT)                                                    \
CLASS##Decl *Decl::get##CLASS##Decl() {                                        \
  assert(Kind == DeclKind::CLASS && "Invalid convertion");                     \
  return static_cast<CLASS##Decl *>(this);                                     \
}
#include "dusk/AST/DeclNodes.def"

Decl::Decl(DeclKind K, StringRef N, SMLoc NL, TypeRepr *TR) : Decl(K, N, NL) {
  TyRepr = TR;
}

bool Decl::isValDecl() const {
  switch (Kind) {
  case DeclKind::Var:
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

ValDecl::ValDecl(DeclKind K, Specifier S, StringRef N, SMLoc NL, Expr *E)
    : Decl(K, N, NL), Value(E), Spec(S) {}

ValDecl::ValDecl(DeclKind K, Specifier S, StringRef N, SMLoc NL, Expr *E,
                 TypeRepr *TR)
    : Decl(K, N, NL, TR), Value(E), Spec(S) {}

// MARK: - VarDecl class

VarDecl::VarDecl(Specifier S, StringRef N, SMLoc NL, SMLoc VarL, Expr *V)
    : ValDecl(DeclKind::Var, S, N, NL, V), VarLoc(VarL) {}

VarDecl::VarDecl(Specifier S, StringRef N, SMLoc NL, SMLoc VarL, Expr *V,
                 TypeRepr *TR)
    : ValDecl(DeclKind::Var, S, N, NL, V, TR), VarLoc(VarL) {}

SMRange VarDecl::getSourceRange() const {
  if (hasValue())
    return {VarLoc, getValue()->getLocEnd()};
  else if (hasTypeRepr())
    return {VarLoc, getTypeRepr()->getLocEnd()};
  else
    return {VarLoc, Decl::getSourceRange().End};
}

// MARK: - ParamDecl class

ParamDecl::ParamDecl(Specifier S, StringRef N, SMLoc NL)
    : ValDecl(DeclKind::Param, S, N, NL, nullptr) {}
ParamDecl::ParamDecl(Specifier S, StringRef N, SMLoc NL, TypeRepr *TR)
    : ValDecl(DeclKind::Param, S, N, NL, nullptr, TR) {}

// MARK: - FuncDecl class

FuncDecl::FuncDecl(StringRef N, SMLoc NL, SMLoc FuncL, VarPattern *A)
    : Decl(DeclKind::Func, N, NL), FuncLoc(FuncL), Params(A) {}

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
