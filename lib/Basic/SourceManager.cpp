//===--- SourceManager.cpp ------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Basic/SourceManager.h"

using namespace dusk;

unsigned dusk::getBufferForLoc(const llvm::SourceMgr &SM, SMLoc Loc) {
  // Validate location
  assert(Loc.isValid());

  auto ptr = Loc.getPointer();
  for (unsigned i = 1; i <= SM.getNumBuffers(); i++) {
    auto Buff = SM.getMemoryBuffer(i);
    if (ptr >= Buff->getBufferStart() && ptr <= Buff->getBufferEnd())
      return i;
  }
  llvm_unreachable("Location in non-existing buffer.");
}
