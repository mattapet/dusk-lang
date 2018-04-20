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

#include "dusk/AST/Diagnostics.h"
#include "dusk/Frontend/InputFile.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SourceMgr.h"
#include <vector>
#include <memory>

namespace dusk {
class ParserResult;

class Compiler {
  llvm::SourceMgr SourceManager;
  diag::Diagnostics Diag;
  std::vector<std::unique_ptr<InputFile>> InputFiles;

public:
  Compiler(std::vector<llvm::StringRef> Filenames);

  void Compile();
  void Lex();

private:
  void consumeDiagnostics();
};

} // namesapce dusk

#endif /* DUSK_COMPILER_H */
