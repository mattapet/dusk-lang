//===--- main.cpp - Dusk source compiler ------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Basic/LLVM.h"
#include "dusk/Frontend/CompilerInvocation.h"
#include "dusk/Frontend/CompilerInstance.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include <string>
#include <iostream>

#include "dusk/AST/ASTContext.h"
#include "dusk/AST/Type.h"

using namespace dusk;
using namespace llvm;

cl::opt<std::string> InFile(cl::Positional, cl::Required,
                            cl::desc("<input file>"), cl::init("-"));

cl::opt<std::string> OutFile("o", cl::desc("Specify output filename"),
                             cl::value_desc("<filename>"), cl::init("a.out"));
cl::opt<bool> IsQuiet("quiet", cl::desc("Suppress diagnostics"));
cl::alias IsQuiet2("q", cl::desc("Suppress diagnostics"),
                   cl::aliasopt(IsQuiet));

cl::opt<bool> OnlyCompile("c",
                          cl::desc("Run parser, type checker as well as LLVM "
                                   "IR generation and create an object file."
                                   "Does not perform linkage of binary"));

cl::opt<bool> PrintIR("S",
                      cl::desc("Print outputed IR of compilation"));

void initCompilerInstance(CompilerInstance &C) {
  CompilerInvocation Inv;
  Inv.setArgs(C.getSourceManager(), C.getDiags(), InFile, OutFile, IsQuiet,
              PrintIR);
  if (!Inv.getInputFile())
    return;
  C.reset(std::move(Inv));
}

int main(int argc, const char *argv[]) {
  cl::ParseCommandLineOptions(argc, argv);
  CompilerInstance Compiler;
  initCompilerInstance(Compiler);
  if (!Compiler.getInputFile()) {
    std::cerr << "File '" + InFile + "' not found.";
    return 1;
  }
  Compiler.performCompilation();
  
  if (!OnlyCompile && !Compiler.getContext().isError()) {
    auto Cmd = "clang++ "
    + Compiler.getInputFile()->file() + ".o "
    + "-L$DUSK_STDLIB_PATH -lstddusk -o" + OutFile;
    system(Cmd.c_str());
    return 0;
  } else {
    return 1;
  }
}
