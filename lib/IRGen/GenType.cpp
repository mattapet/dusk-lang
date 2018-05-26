//===--- GenType.cpp ------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "GenType.h"

#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/Type.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include <vector>

#include "IRGenModule.h"

using namespace dusk;
using namespace irgen;

// MARK: - Type generation

llvm::Type *irgen::codegenIntType(IRGenModule &IRGM, IntType *Ty) {
  return llvm::Type::getInt64Ty(IRGM.LLVMContext);
}

llvm::Type *irgen::codegenVoidType(IRGenModule &IRGM, VoidType *Ty) {
  return llvm::Type::getVoidTy(IRGM.LLVMContext);
}

llvm::Type *irgen::codegenArrayType(IRGenModule &IRGM, ArrayType *Ty) {
  auto BaseTy = codegenType(IRGM, Ty->getBaseType());
  return llvm::ArrayType::get(BaseTy, Ty->getSize());
}

llvm::Type *irgen::codegenInOutType(IRGenModule &IRGM, InOutType *Ty) {
  return codegenType(IRGM, Ty->getBaseType());
}

llvm::Type *codegenFunctionType(IRGenModule &IRGM, FunctionType *Ty) {
  return nullptr;
}

llvm::Type *codegenPatternType(IRGenModule &IRGM, PatternType *Ty) {
  return nullptr;
}

llvm::Type *irgen::codegenType(IRGenModule &IRGM, Type *Ty) {
  switch (Ty->getKind()) {
#define TYPE(CLASS, PARENT)                                                    \
  case TypeKind::CLASS:                                                        \
    return codegen##CLASS##Type(IRGM, static_cast<CLASS##Type *>(Ty));
#include "dusk/AST/TypeNodes.def"
  }
  llvm_unreachable("All cases handeled");
}

// MARK: - Initialization generation

llvm::Constant *irgen::codegenInitInt(IRGenModule &IRGM, IntType *Ty) {
  auto VTy = codegenIntType(IRGM, Ty);
  return llvm::ConstantInt::get(VTy, 0);
}

llvm::Constant *irgen::codegenInitArray(IRGenModule &IRGM, ArrayType *Ty) {
  auto ArrTy = static_cast<llvm::ArrayType *>(codegenArrayType(IRGM, Ty));
  return llvm::ConstantAggregateZero::get(ArrTy);
}

llvm::Constant *irgen::codegenInit(IRGenModule &IRGM, Type *Ty) {
  switch (Ty->getKind()) {
  case TypeKind::Int:
    return codegenInitInt(IRGM, static_cast<IntType *>(Ty));
  case TypeKind::Array:
    return codegenInitArray(IRGM, static_cast<ArrayType *>(Ty));

  default:
    llvm_unreachable("Unexpected type.");
  }
}

// MARK: - Allocation

Address irgen::codegenAllocaInt(IRGenModule &IRGM, IntType *Ty) {
  return IRGM.Builder.CreateAlloca(codegenType(IRGM, Ty));
}

Address irgen::codegenAllocaArray(IRGenModule &IRGM, ArrayType *Ty) {
  return IRGM.Builder.CreateAlloca(codegenType(IRGM, Ty));
}

Address irgen::codegenAlloca(IRGenModule &IRGM, Type *Ty) {
  switch (Ty->getKind()) {
  case TypeKind::Int:
    return codegenAllocaInt(IRGM, static_cast<IntType *>(Ty));
  case TypeKind::Array:
    return codegenAllocaArray(IRGM, static_cast<ArrayType *>(Ty));

  default:
    llvm_unreachable("Unexpected type.");
  }
}
