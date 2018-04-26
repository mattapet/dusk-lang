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

IdentTypeRepr::IdentTypeRepr(SMLoc CL, StringRef N)
    : TypeRepr(TypeReprKind::Ident), ColonLoc(CL), Ident(N) {}

SMRange IdentTypeRepr::getSourceRange() const {
  auto E = Ident.data() + Ident.size();
  return {ColonLoc, SMLoc::getFromPointer(E)};
}
