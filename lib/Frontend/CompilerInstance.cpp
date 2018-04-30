//===--- CompilerInstance.cpp -------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Frontend/CompilerInstance.h"

#include "dusk/AST/Diagnostics.h"
#include "dusk/Parse/Parser.h"
#include "dusk/Sema/Sema.h"
#include "dusk/IRGen/IRGenerator.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include <string>

using namespace dusk;

CompilerInstance::CompilerInstance() { Diag.addConsumer(this); }

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
  performSema();
  if (Context->isError())
    return;
  irgen::IRGenerator Gen(*Context, Diag);
  auto M = Gen.perform();
  M->print(llvm::errs(), nullptr);
  llvm::errs() << "\n";
  
  emitObjectFile(M);
}

void CompilerInstance::performSema() {
  performParseOnly();
  if (Context->isError())
    return;
  sema::Sema S(*Context, Diag);
  S.perform();
}

void CompilerInstance::performParseOnly() {
  Context = std::make_unique<ASTContext>();
  auto InputFile = Invocation.getInputFile();
  Parser P(*Context, SourceManager, *InputFile, Diag, InputFile->bufferID());
  MainModule = P.parseModule();
  Context->setRootModule(MainModule);
}

void CompilerInstance::freeContext() { Context.reset(); }

void CompilerInstance::reset(CompilerInvocation &&I) {
  Invocation = std::move(I);
  MainModule = nullptr;
  freeContext();
}

void CompilerInstance::consume(SMDiagnostic &Diagnostic) {
  Diagnostic.print("duskc", llvm::errs());
}

void CompilerInstance::emitObjectFile(llvm::Module *M) {
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();
  
  std::string Err;
  auto Target =
      llvm::TargetRegistry::lookupTarget(Invocation.getTargetTriple(), Err);
  
  if (!Target) {
    llvm::errs() << Err;
    return;
  }
  
  auto CPU = "generic";
  auto Features = "";
  llvm::TargetOptions Opt;
  auto RM = Optional<llvm::Reloc::Model>();
  auto TargetMachine = Target->createTargetMachine(Invocation.getTargetTriple(),
                                                   CPU, Features, Opt, RM);

  M->setDataLayout(TargetMachine->createDataLayout());
  M->setTargetTriple(Invocation.getTargetTriple());
  
  // Open output file
  auto Filename = "output.o"; // Invocation.getOutputFilename();
  std::error_code EC;
  llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::F_None);
  
  if (EC) {
    llvm::errs() << "Could not open file '" << Filename
                 << "': " << EC.message();
    return;
  }
  
  // Emit pass
  llvm::legacy::PassManager pass;
  auto FileType = llvm::TargetMachine::CGFT_ObjectFile;
  if (TargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
    llvm::errs() << "TargetMachine can't emit a file of this type";
    return;
  }
  llvm::verifyModule(*M, &llvm::errs());
  pass.run(*M);
  dest.flush();
}




