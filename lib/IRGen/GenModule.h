//===--- GenModule.h - Dusk IR generation for module ------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_GEN_MODULE_H
#define DUSK_IRGEN_GEN_MODULE_H

#include "dusk/AST/ASTVisitor.h"
#include "Address.h"
#include "IRGenValue.h"

namespace dusk {
namespace irgen {
class IRGenModule;

void genModule(IRGenModule &IRGM);

} // namesapce irgen
} // namespace dusk

#endif /* DUSK_IRGEN_GEN_MODULE_H */
