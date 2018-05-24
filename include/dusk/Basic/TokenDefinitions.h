//===--- TokenDefinitions.h -------------------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_TOKEN_DEFINITIONS_H
#define DUSK_TOKEN_DEFINITIONS_H

#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

namespace dusk {

enum struct tok {
#define TOKEN(TOK) TOK,
#include "dusk/Basic/TokenDefinitions.def"
};

} // namespace dusk

#endif /* DUSK_TOKEN_DEFINITIONS_H */
