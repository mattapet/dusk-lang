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
  
enum struct TypeKind;

enum struct TypeKind {
  Void,
  Int,
  Value,
  Pattern,
  FuncRet
};

class Type {
  /// Exact kind of the type.
  TypeKind Kind;

public:
  Type(TypeKind K);

  TypeKind getKind() const { return Kind; }

  virtual SMRange getSourceRange() const = 0;
  virtual bool isValueType() const { return false; }
  virtual bool isVoidType() const { return false; }
  virtual bool isClassOf(const Type *T) const { return false; }
  virtual bool isClassOf(const ValueType *T) const { return false; }
  virtual bool isClassOf(const VoidType *T) const { return false; }
  virtual bool isClassOf(const IntType *T) const { return false; }
  virtual bool isClassOf(const FunctionType *T) const { return false; }
  virtual bool isClassOf(const PatternType *T) const { return false; }
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
  virtual bool isVoidType() const override { return false; }
  virtual bool isClassOf(const VoidType *T) const override { return true; }
};

/// Integer type encapsulation.
class IntType : public ValueType {
public:
  IntType();
  virtual bool isClassOf(const IntType *T) const override { return true; }
};
  
class FunctionType : public Type {
  Type *ArgsTy;
  Type *RetTy;
  
public:
  FunctionType(Type *AT, Type *RT);
  Type *getArgsType() const { return ArgsTy; }
  Type *getRetType() const { return RetTy; }
  virtual bool isClassOf(const FunctionType *T) const override;
};


class PatternType : public Type {
  SmallVector<Type *, 128> Items;
public:
  PatternType(SmallVector<Type *, 128> &&I);
  
  ArrayRef<Type *> getItems() const { return Items; }
  virtual bool isClassOf(const PatternType *T) const override;
};
  
  
class FuncRetType : public Type {
  /// Location of arrow operator
  SMLoc ArrowLoc;

  /// The actual return type
  Token RetType;

public: FuncRetType(SMLoc AL, Token RTy);

  SMLoc getArrowLoc() const { return ArrowLoc; }
  Token getRetType() const { return RetType; }

  virtual SMRange getSourceRange() const override;
};
  
} // namespace dusk

#endif /* DUSK_TYPE_H */
