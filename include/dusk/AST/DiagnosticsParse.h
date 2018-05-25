//===--- DiagnosticsParse.h - Parser and Lexer diagnostics ------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_DIAGNOSTICS_PARSE_H
#define DUSK_DIAGNOSTICS_PARSE_H

#include "dusk/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"

namespace dusk {

namespace diag {

enum DiagID : unsigned {
#define DIAG(Id, Text) Id,
#include "dusk/AST/Diagnostics.def"
};

static StringRef getTextForID(DiagID ID) {
  switch (ID) {
#define DIAG(Id, Text) \
  case DiagID::Id: return Text;

#include "dusk/AST/Diagnostics.def"
  }
}

} // namespace diag

} // namespace dusk

#endif /* DUSK_DIAGNOSTICS_PARSE_H */
