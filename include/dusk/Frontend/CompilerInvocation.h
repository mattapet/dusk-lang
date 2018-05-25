//===--- CompilerInvocation.h - Dusk compiler configuration -----*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_COMPILER_INVOCATION_H
#define DUSK_COMPILER_INVOCATION_H

#include "dusk/Basic/LLVM.h"
#include "dusk/AST/Diagnostics.h"
#include "dusk/Frontend/SourceFile.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/SourceMgr.h"
#include <vector>
#include <memory>

namespace dusk {

/// Compiler configuration.
class CompilerInvocation {

  /// Minimum build target
  llvm::Triple Target;

  std::unique_ptr<SourceFile> InputFile;

  StringRef ModuleName;
  StringRef OutputName;

  bool IsQuiet;

public:
  CompilerInvocation();

  void setArgs(SourceMgr &SM, DiagnosticEngine &Diag, StringRef InFile,
               StringRef OutFile, bool IsQuiet);

  StringRef getTargetTriple() const { return Target.str(); }

  SourceFile *getInputFile() const { return InputFile.get(); }

  void setModuleName(StringRef Name) { ModuleName = Name; }

  StringRef getModuleName() const { return ModuleName; }

  StringRef getOutputFilename() const { return OutputName; }
};

} // namespace dusk

#endif /* DUSK_COMPILER_INVOCATION_H */
