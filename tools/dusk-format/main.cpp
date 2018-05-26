//===--- main.cpp - Dusk source formatter -----------------------*- C++ -*-===//
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
#include "dusk/AST/ASTPrinter.h"
#include "dusk/Frontend/Formatter.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_os_ostream.h"
#include <iostream>
#include <string>

#include "dusk/AST/Scope.h"

using namespace dusk;
using namespace llvm;

cl::opt<std::string> InFile(cl::Positional, cl::Required,
                            cl::desc("<input file>"), cl::init("-"));

cl::opt<unsigned> SpaceLen("indent-size",
                           cl::desc("Indenetation size. Aplicable only when "
                                    "idending with spaces. Default indentation"
                                    "is 4 spaces."),
                           cl::init(4));
cl::opt<IndKind>
    IndentationType("indent-type",
                    cl::desc("Indentation type. Choose wheather to ident with "
                             "spaces or tabs. By default formatter indent with"
                             "spaces."),
                    cl::values(clEnumVal(IndKind::Space, "Indent with spaces."),
                               clEnumVal(IndKind::Tab, "Indent with tabs.")),
                    cl::init(IndKind::Space));

cl::opt<bool> IsQuiet("quiet", cl::desc("Suppress diagnostics"));
cl::alias IsQuiet2("q", cl::desc("Suppress diagnostics"),
                   cl::aliasopt(IsQuiet));


void initCompilerInstance(CompilerInstance &C) {
  CompilerInvocation Inv;
  Inv.setArgs(C.getSourceManager(), C.getDiags(), InFile, "", IsQuiet,
              false);
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
  
  Compiler.performParseOnly();
  if (Compiler.hasASTContext() && !Compiler.getContext().isError()) {
    llvm::raw_os_ostream OS(std::cout);
    Formatter F(Compiler.getContext(), OS);
    F.setIndentSize(SpaceLen);
    F.setIndentType(IndentationType);
    F.format();
    return 0;
  } else {
    return 1;
  }
}
