#include "dusk/Frontend/Compiler.h"
#include "dusk/Frontend/Formatter.h"
#include "dusk/Parse/Lexer.h"
#include "dusk/Parse/TokenDefinition.h"
#include "dusk/Parse/Parser.h"
#include "llvm/Support/raw_os_ostream.h"

#include <iostream>

using namespace dusk;

Compiler::Compiler(std::vector<llvm::StringRef> Filenames)
: DiagEngine(SourceManager) {
    for (auto &F : Filenames) {
        auto File = std::make_unique<InputFile>(SourceManager, F);
        InputFiles.push_back(std::move(File));
    }
}

void Compiler::Compile() {
    Formatter F;
    llvm::raw_os_ostream OS(std::cout);
    
    for (auto &&File : InputFiles) {
        Parser P(SourceManager, DiagEngine, *File, File->bufferID());
        F.format(P.parse(), OS);
    }
}

void Compiler::Lex() {
    for (auto &&File : InputFiles) {
        Lexer L(SourceManager, File->bufferID(), true);
        Token T;
        do {
            L.lex(T);
            if (T.is(tok::unknown)) {
                L.diagnose(T);
                return;
            }
//            std::cout << T.ToString()
//            << " `" << T.getText().str() << "`"
//            << std::endl;
        } while (T.isNot(tok::eof));
    }
}

