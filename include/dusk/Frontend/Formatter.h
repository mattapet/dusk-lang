//===--- Formatter.h - Dusk language formatter ------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_FORMATTER_H
#define DUSK_FORMATTER_H

#include "dusk/AST/ASTNode.h"
#include "llvm/Support/raw_ostream.h"

namespace dusk {

class Formatter {
public:
  void format(ASTNode *N, llvm::raw_ostream &OS);
};

} // namespace dusk

#endif /* DUSK_FORMATTER_H */
