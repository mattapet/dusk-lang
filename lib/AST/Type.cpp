//===--- Type.cpp ---------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/Type.h"
#include "dusk/AST/ASTContext.h"

using namespace dusk;

Type::Type(TypeKind K) : Kind(K) {}

#define TYPE(CLASS, PARENT)                                                    \
CLASS##Type *Type::get##CLASS##Type() {                                        \
  assert(Kind == TypeKind::CLASS && "Invalid conversion");                     \
  return static_cast<CLASS##Type *>(this);                                     \
}
#include "dusk/AST/TypeNodes.def"

void *Type::operator new(size_t Bytes, ASTContext &Context) {
  return Context.Allocate(Bytes);
}

ValueType::ValueType(TypeKind K) : Type(K) {}

VoidType::VoidType() : Type(TypeKind::Void) {}

IntType::IntType() : ValueType(TypeKind::Int) {}

// MARK: - Array type

ArrayType::ArrayType(Type *BT, size_t S)
    : ValueType(TypeKind::Array), BaseTy(BT), Size(S) {}

bool ArrayType::isClassOf(const ArrayType *T) const {
  return BaseTy->isClassOf(T->getBaseType()) && Size == T->getSize();
}

// MARK: - Function type

FunctionType::FunctionType(Type *AT, Type *RT)
    : Type(TypeKind::Function), ArgsTy(AT), RetTy(RT) {}

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
