//===--- IRGenValue.h - Value representation --------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_IRGEN_VALUE_H
#define DUSK_IRGEN_IRGEN_VALUE_H

#include "Address.h"

namespace llvm {
class Value;
} // namespace llvm

namespace dusk {
class Type;

namespace irgen {

/// Represents a single rvalue reference.
class RValue {
  llvm::Value *Value;

  Type *Ty;

public:
  RValue();
  
  static RValue get(Type *Ty, llvm::Value *V);

private:
  RValue(const RValue &) = delete;
  RValue &operator=(const RValue &) = delete;
};


/// Represents a single lvalue refenrece.
class LValue {
  enum KindType { Simple, ArrayElement };

  KindType Kind;

  /// Encapsulated raw value address.
  llvm::Value *Value;

  /// Type of value.
  Type *Ty;
  
  /// Index of referenced element in array.
  llvm::Value *ArrIdx;

public:
  /// Creates an empty invalid lvalue.
  LValue();
  /// Creates a siple lvalue
  LValue(Type *Ty, Address Addr);
  /// Creates a array access lvalue.
  LValue(Type *Ty, Address Addr, llvm::Value *Idx);
  /// Moving constructor for copying of lvalues.
  LValue(LValue &&);
  LValue &operator=(LValue &&);

  operator bool() const { return isValid(); }
  
  /// Return \c true if type and value are set, \c false otherwise.
  bool isValid() const { return Ty != nullptr && Value != nullptr; }
  
  bool isSimple() const { return Kind == Simple; }
  bool isArrayElement() const { return Kind == ArrayElement; }

  /// Sets raw address of referenced value.
  void setAddress(Address Addr) { Value = Addr; }
  
  /// Returns raw address of referenced value.
  Address getAddress() const { return Value; }
  
  /// Sets type of referenced value.
  void setType(Type *T) { Ty = T; }
  
  /// Returns raw address of referenced value.
  Type *getType() const { return Ty; }
  
  /// Returns pointer to the referenced value.
  ///
  /// \note Referenced value must be a simple value.
  llvm::Value *getPointer() const {
    assert(isSimple() && "Invalid address access.");
    return Value;
  }

  /// Returns pointer to the start of the array, which contains referenced
  /// element.
  ///
  /// \note Referened value must be an array element.
  llvm::Value *getArrayPtr() const {
    assert(isArrayElement() && "Invalid address access.");
    return Value;
  }
  
  /// Return index of referenced value as value.
  ///
  /// \note Referened value must be an array element.
  llvm::Value *getElementIndex() const {
    assert(isArrayElement() && "Invalid address access.");
    return Value;
  }

  // MARK: - static construction methods
  
  /// Creates and returns a simple lvalue.
  static LValue getVal(Type *Ty, Address Val);
  
  /// Creates and returns an array element lvalue.
  static LValue getArrayElem(Type *Ty, Address Val, llvm::Value *Idx);

private:
  LValue(const LValue &) = delete;
  LValue &operator=(const LValue &) = delete;
};

} // namespace irgen
} // namespace dusk

#endif /* DUSK_IRGEN_IRGEN_VALUE_H */
