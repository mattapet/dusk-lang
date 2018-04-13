#include "dusk/Frontend/Compiler.h"
#include "llvm/ADT/StringRef.h"
#include <iostream>
#include <vector>

using namespace dusk;

int main(int argc, const char *argv[]) {
    std::vector<llvm::StringRef> Filenames;
    for (size_t i = 1; i < argc; i++) {
        Filenames.push_back(argv[1]);
    }
    Compiler C(Filenames);
    C.Lex();

    return 0;
}
