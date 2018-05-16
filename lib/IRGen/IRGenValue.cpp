//===--- IRGenValue.cpp ---------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "IRGenValue.h"

#include "dusk/AST/Type.h"
#include "llvm/IR/Value.h"

using namespace dusk;
using namespace irgen;

// MARK: - rvalue

RValue::RValue() : Value(nullptr), Ty(nullptr) {}

// MARK: - lvalue

LValue::LValue()
    : Kind(LValue::Simple), Value(nullptr), Ty(nullptr), ArrIdx(nullptr) {}

LValue::LValue(Type *Ty, Address Addr)
    : Kind(LValue::Simple), Value(Addr), Ty(Ty), ArrIdx(nullptr) {}

LValue::LValue(Type *Ty, Address Addr, llvm::Value *Idx)
  : Kind(LValue::ArrayElement), Value(Addr), Ty(Ty), ArrIdx(Idx) {}

LValue::LValue(LValue &&Other)
    : Kind(Other.Kind), Value(Other.Value), Ty(Other.Ty), ArrIdx(Other.ArrIdx) {
  assert(isValid() && "Copy of invalid l-value");
}

LValue &LValue::operator=(LValue &&Other) {
  assert(!isValid() && "Overriding valid value");
  Kind = Other.Kind;
  Value = Other.Value;
  Ty = Other.Ty;
  ArrIdx = Other.ArrIdx;
  return *this;
}

LValue LValue::getVal(Type *Ty, Address Val) {
  LValue Ret(Ty, Val);
  Ret.Kind = LValue::Simple;
  return Ret;
}

LValue LValue::getArrayElem(Type *Ty, Address Val, llvm::Value *Idx) {
  LValue Ret(Ty, Val, Idx);
  return Ret;
}

