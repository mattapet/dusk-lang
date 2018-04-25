#include "dusk/Frontend/Compiler.h"
#include "dusk/Frontend/Formatter.h"
#include "dusk/Parse/Lexer.h"
#include "dusk/Basic/TokenDefinition.h"
#include "dusk/Parse/Parser.h"
#include "dusk/IRGen/IRGenerator.h"
#include "llvm/Support/raw_os_ostream.h"

#include "llvm/Analysis/Passes.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <vector>

using namespace dusk;

Compiler::Compiler(std::vector<StringRef> Filenames)
  : Engine(SourceManager), OS(raw_os_ostream(std::cerr)) {
  Engine.addConsumer(this);
  for (auto &F : Filenames) {
    auto File = std::make_unique<InputFile>(SourceManager, F);
    InputFiles.push_back(std::move(File));
  }
}

void Compiler::Compile() {
  Formatter F;
  raw_os_ostream OS(std::cout);
  std::vector<ParserResult> Results;

  for (auto &&File : InputFiles) {
    Parser P(SourceManager, *File, Engine, File->bufferID());
    auto R = P.parse();
    // Stop compilation after error encounterment
    if (R.isError())
      return;
    Results.push_back(std::move(R));
  }

//  for (auto &&R : Results)
//    F.format(R.getRoot(), OS);
  irgen::IRGenerator IRGen(Engine);
  auto Module = IRGen.gen(Results.front().getRoot());
  if (!Module)
    return;
  
  Module->print(llvm::errs(), nullptr);
  llvm::verifyModule(*Module);
  std::cout << std::endl;
}

void Compiler::Lex() {
  for (auto &&File : InputFiles) {
    Lexer L(SourceManager, File->bufferID(), &Engine, true);
    Token T;
    do {
      L.lex(T);
      if (T.is(tok::unknown)) {
        return;
      }
    } while (T.isNot(tok::eof));
  }
}

void Compiler::consume(llvm::SMDiagnostic &Diag) {
  Diag.print("duskc", OS);
}

