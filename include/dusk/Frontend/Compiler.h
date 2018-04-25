//===--- Compiler.h - Dusk compiler class -----------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_COMPILER_H
#define DUSK_COMPILER_H

#include "dusk/Basic/LLVM.h"
#include "dusk/AST/Diagnostics.h"
#include "dusk/AST/Diagnostics.h"
#include "dusk/Frontend/InputFile.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_os_ostream.h"
#include <vector>
#include <memory>

namespace llvm {
class Module;
}

namespace dusk {
class ParserResult;

class Compiler : public DiagnosticConsumer {
  llvm::SourceMgr SourceManager;

  DiagnosticEngine Engine;

  std::vector<std::unique_ptr<InputFile>> InputFiles;

  /// Diagnostic output stream
  llvm::raw_os_ostream OS;

public:
  Compiler(std::vector<StringRef> Filenames);

  void Compile();
  void Lex();

  void consume(SMDiagnostic &Diag);
};

} // namesapce dusk

#endif /* DUSK_COMPILER_H */
