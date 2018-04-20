#include "dusk/Frontend/Compiler.h"
#include "dusk/Frontend/Formatter.h"
#include "dusk/Parse/Lexer.h"
#include "dusk/Parse/TokenDefinition.h"
#include "dusk/Parse/Parser.h"
#include "llvm/Support/raw_os_ostream.h"

#include <iostream>
#include <vector>

using namespace dusk;

Compiler::Compiler(std::vector<llvm::StringRef> Filenames) {
  for (auto &F : Filenames) {
    auto File = std::make_unique<InputFile>(SourceManager, F);
    InputFiles.push_back(std::move(File));
  }
}

void Compiler::Compile() {
  Formatter F;
  llvm::raw_os_ostream OS(std::cout);
  std::vector<ParserResult> Results;

  for (auto &&File : InputFiles) {
    Parser P(SourceManager, *File, Diag, File->bufferID());
    Results.push_back(P.parse());
    if (!Diag.isEmpty())
      return consumeDiagnostics();
  }

  for (auto &&R : Results)
    F.format(R.getRoot(), OS);
}

void Compiler::Lex() {
  for (auto &&File : InputFiles) {
    Lexer L(SourceManager, File->bufferID(), &Diag, true);
    Token T;
    do {
      L.lex(T);
      if (T.is(tok::unknown)) {
        return;
      }
    } while (T.isNot(tok::eof));
  }
}

void Compiler::consumeDiagnostics() {
  llvm::raw_os_ostream OS(std::cerr);
  Diag.consume(OS);
}


