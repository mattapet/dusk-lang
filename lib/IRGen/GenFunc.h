//===--- GenFunc.h - Function generation state ------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_GEN_FUNC_H
#define DUSK_IRGEN_GEN_FUNC_H

#include "dusk/Basic/LLVM.h"

#include "Address.h"

namespace llvm {
  
class Function;
class FunctionType;
class BasicBlock;

} // namespace llvm

namespace dusk {
class Decl;
class Stmt;
class FuncDecl;
class FuncStmt;
class BlockStmt;
class Type;
class FunctionType;
  
namespace irgen {
class IRGenFunc;
  
bool genFunc(IRGenFunc &IRGF, FuncStmt *F);
  
  
} // namespace irgen
} // namespace dusk

#endif /* DUSK_IRGEN_GEN_FUNC_H */
