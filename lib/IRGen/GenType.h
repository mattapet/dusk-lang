//===--- GenType.h - Dusk IR type generation --------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_GEN_TYPE_H
#define DUSK_IRGEN_GEN_TYPE_H

#include "Address.h"

namespace llvm {
class Type;
class Value;
class Constant;
}

namespace dusk {
class Type;
class VoidType;
class IntType;
class ArrayType;
  
namespace irgen {
class IRGenModule;
class IRGenFunc;
  
llvm::Type *codegenIntType(IRGenModule &IRGM, IntType *Ty);
llvm::Type *codegenVoidType(IRGenModule &IRGM, VoidType *Ty);
llvm::Type *codegenArrayType(IRGenModule &IRGM, ArrayType *Ty);
llvm::Type *codegenType(IRGenModule &IRGM, Type *Ty);

llvm::Constant *codegenInitInt(IRGenModule &IRGM, IntType *Ty);
llvm::Constant *codegenInitArray(IRGenModule &IRGM, ArrayType *Ty);
llvm::Constant *codegenInit(IRGenModule &IRGM, Type *Ty);
  
Address codegenAllocaInt(IRGenModule &IRGM, IntType *Ty);
Address codegenAllocaArray(IRGenModule &IRGM, ArrayType *Ty);
Address codegenAlloca(IRGenModule &IRGM, Type *Ty);

  
} // namespace irgen
} // namespace dusk

#endif /* DUSK_IRGEN_GEN_TYPE_H */
