//===--- Type.h - Dusk type nodes ------------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_TYPE_H
#define DUSK_TYPE_H

#include "dusk/Basic/LLVM.h"
#include "dusk/Parse/Token.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/SMLoc.h"

namespace dusk {
class Type;
class ValueType;
class VoidType;
class IntType;
class FunctionType;
class PatternType;
class ArrayType;
class ASTContext;
  
enum struct TypeKind;

enum struct TypeKind { Void, Int, Value, Pattern, Array, Function };

class Type {
  /// Exact kind of the type.
  TypeKind Kind;

public:
  Type(TypeKind K);

  TypeKind getKind() const { return Kind; }

  virtual bool isValueType() const { return false; }
  virtual bool isVoidType() const { return false; }
  virtual bool isClassOf(const Type *T) const { return this == T; }
  
  VoidType *getVoidType();
  IntType *getIntType();
  FunctionType *getFuncType();
  ArrayType *getArrayType();
  
private:
  void *operator new(size_t Bytes) throw() = delete;
  void operator delete(void *Data) throw() = delete;
  
public:
  /// Only allow allocation using \c ASTContext
  void *operator new(size_t Bytes, ASTContext &Context);
  void *operator new(size_t Bytes, void *Mem) throw() { return Mem; }
};

class ValueType : public Type {
public:
  ValueType(TypeKind K);
  virtual bool isValueType() const override { return true; }
};

/// Void type encapsulation.
class VoidType : public Type {
public:
  VoidType();
  bool isVoidType() const override { return true; }
  bool isClassOf(const Type *T) const override {
    if (Type::isClassOf(T))
      return true;
    return T->isVoidType();
  }
};

/// Integer type encapsulation.
class IntType : public ValueType {
public:
  IntType();
  bool isClassOf(const Type *T) const override {
    if (Type::isClassOf(T))
      return true;
    return T->getKind() == TypeKind::Int;
  }
};
  
/// Representing array type
class ArrayType : public ValueType {
  Type *BaseTy;
  size_t Size;
  
public:
  ArrayType(Type *Ty, size_t S);
  
  Type *getBaseType() const { return BaseTy; }
  size_t getSize() const { return Size; }
  
  bool isClassOf(const Type *T) const  override {
    if (Type::isClassOf(T))
      return true;
    if (auto Ty = dynamic_cast<const ArrayType *>(T))
      return isClassOf(Ty);
    return false;
  }
  
  bool isClassOf(const ArrayType *T) const;
};

/// Representing function type
class FunctionType : public Type {
  Type *ArgsTy;
  Type *RetTy;

public:
  FunctionType(Type *AT, Type *RT);
  Type *getArgsType() const { return ArgsTy; }
  Type *getRetType() const { return RetTy; }
  
  bool isClassOf(const Type *T) const override {
    if (Type::isClassOf(T))
      return true;
    if (auto Ty = dynamic_cast<const FunctionType *>(T))
      return isClassOf(Ty);
    return false;
  }
  
  bool isClassOf(const FunctionType *T) const;
};

class PatternType : public Type {
  SmallVector<Type *, 128> Items;

public:
  PatternType(SmallVector<Type *, 128> &&I);

  ArrayRef<Type *> getItems() const { return Items; }
  
  bool isClassOf(const Type *T) const override {
    if (Type::isClassOf(T))
      return true;
    if (auto Ty = dynamic_cast<const PatternType *>(T))
      return isClassOf(Ty);
    return false;
  }
  bool isClassOf(const PatternType *T) const;
};

} // namespace dusk

#endif /* DUSK_TYPE_H */
