//===--- TypeRepr.cpp -----------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/TypeRepr.h"

using namespace dusk;

TypeRepr::TypeRepr(TypeReprKind K) : Kind(K) {}

// MARK: - Identifier type representation

IdentTypeRepr::IdentTypeRepr(SMLoc CL, StringRef N)
    : TypeRepr(TypeReprKind::Ident), ColonLoc(CL), Ident(N) {}

SMRange IdentTypeRepr::getSourceRange() const {
  auto E = Ident.data() + Ident.size();
  return {ColonLoc, SMLoc::getFromPointer(E)};
}

// MARK: - Function return type

FuncRetTypeRepr::FuncRetTypeRepr(SMLoc AL, StringRef ID)
    : TypeRepr(TypeReprKind::FuncRet), ArrowLoc(AL), Ident(ID) {}

SMRange FuncRetTypeRepr::getSourceRange() const {
  auto E = Ident.data() + Ident.size();
  return {ArrowLoc, SMLoc::getFromPointer(E)};
}
