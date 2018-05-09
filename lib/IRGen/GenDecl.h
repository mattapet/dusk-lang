//===--- GenDecl.h - Dusk IR decalration generation -------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_GEN_DECL_H
#define DUSK_IRGEN_GEN_DECL_H

#include "Address.h"

namespace llvm {
class Value;
}

namespace dusk {
class Decl;
class ValDecl;
class Type;
class ArrayType;
class IntType;
  
namespace irgen {
class IRGenModule;
  
Address codegenDeclGlobal(IRGenModule &IRGM, Decl *D);
Address codegenDeclLocal(IRGenModule &IRGM, Decl *D);
Address codegenDecl(IRGenModule &IRGM, Decl *D);

  
} // namespace irgen
} // namespace dusk

#endif /* DUSK_IRGEN_GEN_DECL_H */
