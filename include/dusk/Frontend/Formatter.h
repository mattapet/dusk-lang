//===--- Formatter.h - Dusk foramtter ---------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_FORMATTER_H
#define DUSK_FORMATTER_H

#include "dusk/Basic/LLVM.h"
#include "dusk/AST/ASTContext.h"
#include "llvm/Support/raw_ostream.h"

namespace dusk {
  
class Formatter {
  ASTContext &Ctx;
  raw_ostream &OS;
  
public:
  Formatter(ASTContext &C, raw_ostream &OS): Ctx(C), OS(OS) {}
  
  void format();
};
  
} // namespace dusk

#endif /* DUSK_FORMATTER_H */

