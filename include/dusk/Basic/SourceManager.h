//===--- SourceManager.h - Convenience functions ----------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_SOURCE_MANAGER_H
#define DUSK_SOURCE_MANAGER_H

#include "llvm/Support/SourceMgr.h"
#include <cassert>

namespace dusk {
  
/// \brief Returns an ID of the buffer containing provided \c Loc.
///
/// \param SM Source manager providing buffers for context.
///
/// \param Loc Location refering to a buffer.
///
/// \return ID of buffer containing provided location.
static unsigned getBufferForLoc(const llvm::SourceMgr &SM, llvm::SMLoc Loc) {
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
  
} // namespace dusk

#endif /* DUSK_SOURCE_MANAGER_H */

