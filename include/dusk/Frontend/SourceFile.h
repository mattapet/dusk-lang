//===--- SourceFile.h -------------------------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_INPUT_FILE_H
#define DUSK_INPUT_FILE_H

#include "dusk/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/MemoryBuffer.h"
#include <string>

namespace dusk {

/// \brief Encapsulation of inpur source file.
///
/// Upon construction of a new \c InputFile class, the object tries to load
/// the provided file and pass it to provided \c SourceMgr instance,
/// to register the as a new source buffer.
class SourceFile {
  unsigned BufferID;
  std::string Filename;
  llvm::MemoryBuffer *Buffer;

public:
  /// \brief Opens and reads a file with provided name.
  SourceFile(unsigned ID, llvm::MemoryBuffer *B, StringRef F)
      : BufferID(ID), Filename(F), Buffer(B) {
    assert(!Filename.empty() && "Filename must not be an empty string.");
  }

  /// \bried Returns an ID of the opened buffer.
  unsigned bufferID() const { return BufferID; }
  llvm::MemoryBuffer *buffer() const { return Buffer; }
  const std::string &file() const { return Filename; }
};

} // namespace dusk

#endif /* DUSK_INPUT_FILE_H */
