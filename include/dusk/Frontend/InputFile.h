//===--- InputFile.h --------------------------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a mila-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/MemoryBuffer.h"
#include <string>

#ifndef File_h
#define File_h

namespace dusk {


/// \brief Encapsulation of inpur source file.
///
/// Upon construction of a new \c InputFile class, the object tries to load
/// the provided file and pass it to provided \c SourceMgr instance,
/// to register the as a new source buffer.
class InputFile {
    unsigned BufferID;
    std::string Filename;
    llvm::MemoryBuffer *Buffer;
    llvm::SourceMgr &SourceManager;

public:
    /// \brief Opens and reads a file with provided name.
    InputFile(llvm::SourceMgr &SM, const llvm::StringRef F);

    /// \bried Returns an ID of the opened buffer.
    unsigned bufferID() const { return BufferID; }
    llvm::MemoryBuffer *buffer() const { return Buffer; }
    const std::string &file() const { return Filename; }
};

} /* namespace mila */

#endif /* File_h */
