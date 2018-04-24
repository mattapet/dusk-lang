#include "dusk/Frontend/Compiler.h"
#include "dusk/Frontend/Formatter.h"
#include "dusk/Parse/Lexer.h"
#include "dusk/Basic/TokenDefinition.h"
#include "dusk/Parse/Parser.h"
#include "dusk/IRGen/IRGenerator.h"
#include "llvm/Support/raw_os_ostream.h"

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
  IRGen.gen(Results.front().getRoot());
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

