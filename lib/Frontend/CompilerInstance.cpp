//===--- CompilerInstance.cpp -------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Frontend/CompilerInstance.h"

#include "dusk/Parse/Parser.h"

using namespace dusk;

ModuleDecl *CompilerInstance::getModule() {
  if (hasASTContext() && !Context->isError())
    return MainModule;
  else
    return nullptr;
}

SourceFile *CompilerInstance::getInputFile() {
  return Invocation.getInputFile();
}

void CompilerInstance::performCompilation() {
  llvm_unreachable("Not implemented");
}

void CompilerInstance::performSema() {
  llvm_unreachable("Not implemented");
}

void CompilerInstance::performParseOnly() {
  Context = std::make_unique<ASTContext>();
  auto InputFile = Invocation.getInputFile();
  Parser P(*Context, SourceManager, *InputFile, Diag, InputFile->bufferID());
  MainModule = P.parseModule();
  Context->setRootModule(MainModule);
}

void CompilerInstance::freeContext() {
  Context.reset();
}

void CompilerInstance::reset(CompilerInvocation &&I) {
  Invocation = std::move(I);
  MainModule = nullptr;
  freeContext();
}
