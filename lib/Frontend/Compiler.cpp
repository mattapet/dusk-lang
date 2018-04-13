#include "dusk/Frontend/Compiler.h"
#include "dusk/Parser/Lexer.h"
#include "dusk/Parser/TokenDefinition.h"

#include <iostream>

using namespace dusk;

Compiler::Compiler(std::vector<llvm::StringRef> Filenames) {
    for (auto &F : Filenames) {
        auto File = std::make_unique<InputFile>(SourceManager, F);
        InputFiles.push_back(std::move(File));
    }
}

void Compiler::Compile() {

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
            std::cout << T.ToString()
            << " `" << T.getText().str() << "`"
            << std::endl;
        } while (T.isNot(tok::eof));
    }
}

