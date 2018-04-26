//===--- Type.cpp ---------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/Type.h"

using namespace dusk;

Type::Type(TypeKind K) : Kind(K) {}

ValueType::ValueType(TypeKind K) : Type(K) {}

VoidType::VoidType() : Type(TypeKind::Void) {}

IntType::IntType() : ValueType(TypeKind::Int) {}


FuncRetType::FuncRetType(SMLoc AL, Token RTy)
    : Type(TypeKind::FuncRet), ArrowLoc(AL), RetType(RTy) {}

SMRange FuncRetType::getSourceRange() const {
  return { ArrowLoc, RetType.getRange().End };
}
