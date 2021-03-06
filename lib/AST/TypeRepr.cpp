//===--- TypeRepr.cpp -----------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/TypeRepr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/ASTContext.h"

using namespace dusk;

TypeRepr::TypeRepr(TypeReprKind K) : Kind(K), Ty(nullptr) {}

#define TYPE_REPR(CLASS, PARENT)                                               \
CLASS##TypeRepr *TypeRepr::get##CLASS##TypeRepr() {                            \
  assert(Kind == TypeReprKind::CLASS && "Invalid conversion");                 \
  return static_cast<CLASS##TypeRepr *>(this);                                 \
}
#include "dusk/AST/TypeReprNodes.def"

void *TypeRepr::operator new(size_t Bytes, ASTContext &Context) {
  return Context.Allocate(Bytes);
}

// MARK: - Identifier type representation

IdentTypeRepr::IdentTypeRepr(StringRef N)
    : TypeRepr(TypeReprKind::Ident), Ident(N) {}

SMRange IdentTypeRepr::getSourceRange() const {
  auto S = Ident.data();
  auto E = S + Ident.size();
  return {SMLoc::getFromPointer(S), SMLoc::getFromPointer(E)};
}

// MARK: - Array type representation

ArrayTypeRepr::ArrayTypeRepr(TypeRepr *B, Stmt *S)
    : TypeRepr(TypeReprKind::Array), BaseTyRepr(B), Size(S) {}

SMRange ArrayTypeRepr::getSourceRange() const {
  return {BaseTyRepr->getLocStart(), Size->getLocEnd()};
}

// MARK: - Inout type representation

InOutTypeRepr::InOutTypeRepr(TypeRepr *B, SMLoc IL)
    : TypeRepr(TypeReprKind::InOut), BaseTyRepr(B), InOutLoc(IL) {}

SMRange InOutTypeRepr::getSourceRange() const {
  return {InOutLoc, BaseTyRepr->getLocEnd()};
}
