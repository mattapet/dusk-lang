//===--- CompilerInvocation.cpp -------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Frontend/CompilerInvocation.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/LineIterator.h"
#include "llvm/Support/Path.h"

using namespace dusk;

CompilerInvocation::CompilerInvocation()
    : Target(llvm::sys::getDefaultTargetTriple()), OutputName("a.out") {}

void CompilerInvocation::setArgs(SourceMgr &SM, DiagnosticEngine &Diag,
                                 StringRef InFile, StringRef OutFile,
                                 bool IsQ) {
  OutputName = OutFile;
  IsQuiet = IsQ;

  if (auto Err = llvm::MemoryBuffer::getFile(InFile)) {
    auto Buff = std::move(*Err);
    auto L = SMLoc::getFromPointer(Buff->getBufferStart());
    auto BuffPtr = Buff.get();
    auto BuffID = SM.AddNewSourceBuffer(std::move(Buff), L);
    InputFile = std::make_unique<SourceFile>(BuffID, BuffPtr, InFile);
  } else {
    llvm_unreachable("Error opening or reading file");
  }
}
