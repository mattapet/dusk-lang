#include "dusk/Frontend/InputFile.h"

using namespace dusk;

InputFile::InputFile(llvm::SourceMgr &SM, const llvm::StringRef F)
    : Filename(F), SourceManager(SM) {
  assert(!Filename.empty() && "Filename must not be an empty string.");

  if (auto Res = llvm::MemoryBuffer::getFile(Filename)) {
    auto B = std::move(*Res);
    auto L = llvm::SMLoc::getFromPointer(B->getBufferStart());
    Buffer = B.get();
    BufferID = SourceManager.AddNewSourceBuffer(std::move(B), L);

  } else {
    assert("Error opening and/or reading file" && false);
  }
}


