//===--- LLVM.h - LLVM imports and declarations -----------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_LLVM_H
#define DUSK_LLVM_H

#include "llvm/ADT/None.h"

namespace llvm {

// ADTs
class StringRef;
class Twine;
template <typename T> class ArrayRef;
template <unsigned InternalLen> class SmallString;
template <typename T, unsigned N> class SmallVector;
template <typename T> class Optional;

// Support
class SMLoc;
class SMRange;
class SourceMgr;
class SMDiagnostic;
class SMFixIt;

// Streams
class raw_ostream;
class raw_os_ostream;

} // namespace llvm

namespace dusk {

// ADTs
using llvm::StringRef;
using llvm::Twine;
using llvm::ArrayRef;
using llvm::SmallString;
using llvm::SmallVector;
using llvm::Optional;
  
// Support
using llvm::SMLoc;
using llvm::SMRange;
using llvm::SourceMgr;
using llvm::SMDiagnostic;
using llvm::SMFixIt;

// Streams
using llvm::raw_ostream;
using llvm::raw_os_ostream;

} // namespace dusk

#endif /* DUSK_LLVM_H */
