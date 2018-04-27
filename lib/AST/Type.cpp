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

// MARK: - Function type

FunctionType::FunctionType(Type *AT, Type *RT)
    : Type(TypeKind::FuncRet), ArgsTy(AT), RetTy(RT) {}

bool FunctionType::isClassOf(const FunctionType *T) const {
  return ArgsTy->isClassOf(T->ArgsTy) && RetTy->isClassOf(T->RetTy);
}

// MARK: - Pattern type

PatternType::PatternType(SmallVector<Type *, 128> &&I)
    : Type(TypeKind::Pattern), Items(I) {}

bool PatternType::isClassOf(const PatternType *T) const {
  if (Items.size() != T->Items.size())
    return false;
  for (size_t i = 0; i < Items.size(); i++) {
    if (!Items[i]->isClassOf(T->Items[i]))
      return false;
  }
  return true;
}

// ====----
FuncRetType::FuncRetType(SMLoc AL, Token RTy)
    : Type(TypeKind::FuncRet), ArrowLoc(AL), RetType(RTy) {}

SMRange FuncRetType::getSourceRange() const {
  return {ArrowLoc, RetType.getRange().End};
}
